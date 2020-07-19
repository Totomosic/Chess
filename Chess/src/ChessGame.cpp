#include "ChessGame.h"

namespace Chess
{

	ChessGame::ChessGame(Window* window, const SceneData& sceneData)
		: m_Window(window), m_SceneData(sceneData), m_Camera(), m_Background(), m_Board(), m_BoardGraphics(sceneData.BoardLayer, sceneData.PiecesLayer, &m_Board, { 100, 100 }, { 0, 0, 0 }),
		m_PreviousMoveMarkers(sceneData.PreviousMoveLayer, &m_BoardGraphics), m_ValidMoveMarkers(sceneData.ValidMovesLayer, &m_BoardGraphics), m_Analyzer(sceneData.UILayer, &m_BoardGraphics),
		m_Players(&m_Board), m_WindowResizeListener(), m_MouseMoveListener(), m_SelectedPieceId(-1), m_SelectedPieceSquare(Boxfish::INVALID_SQUARE)
	{
		m_Camera = m_SceneData.Scene->GetFactory().Camera(Matrix4f::Orthographic(0, m_Window->Width(), 0, m_Window->Height(), -100, 100));
		m_SceneData.BoardLayer->SetActiveCamera(m_Camera);
		m_SceneData.PreviousMoveLayer->SetActiveCamera(m_Camera);
		m_SceneData.ValidMovesLayer->SetActiveCamera(m_Camera);
		m_SceneData.PiecesLayer->SetActiveCamera(m_Camera);
		m_SceneData.UILayer->SetActiveCamera(m_Camera);

		Vector2f windowSize = { (float)m_Window->Width(), (float)m_Window->Height() };
		float boardSize = std::min(windowSize.x, windowSize.y);

		m_BoardGraphics.SetSize({ boardSize, boardSize });
		m_BoardGraphics.SetPosition({ m_Window->Width() / 2.0f, m_Window->Height() / 2.0f, 0.0f });

		UpdateAllGraphics();
		SetupEventListeners();
	}

	void ChessGame::Start()
	{
		SceneManager::Get().SetCurrentScenePtr(m_SceneData.Scene);
		m_Board.SetStartingPosition();
		// m_Board.SetPositionFromFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/R1BQKBNR w KQkq - 0 1");
	}

	void ChessGame::Update()
	{
		if (Input::Get().KeyPressed(Keycode::Esc))
		{
			if (m_OnExit)
				m_OnExit();
		}

		if (Input::Get().KeyPressed(Keycode::Left))
		{
			m_Board.PopMove();
		}
		if (Input::Get().KeyPressed(Keycode::F))
		{
			std::cout << m_Board.GetPosition() << std::endl;
			std::cout << "Fen: " << Boxfish::GetFENFromPosition(m_Board.GetPosition()) << std::endl;
			std::cout << "Hash: " << std::hex << m_Board.GetPosition().Hash.Hash << std::dec << std::endl;

			m_BoardGraphics.Flip();
			m_Analyzer.Flip();
			UpdateAllGraphics();
		}
		if (Input::Get().MouseButtonPressed(MouseButton::Left) && !HasSelectedPiece())
		{
			Boxfish::Square square = m_BoardGraphics.ScreenPositionToSquare(Input::Get().MousePosition().xy());
			if (square != Boxfish::INVALID_SQUARE)
			{
				int pieceId = m_Board.GetPieceIdAt(square);
				if (pieceId >= 0)
				{
					m_SelectedPieceId = pieceId;
					m_SelectedPieceSquare = square;
					DrawPieceAtPosition(m_SelectedPieceId, Input::Get().MousePosition());
					DrawValidMoves(m_Board.GetLegalMovesFor(square));
				}
			}
		}
		if (Input::Get().MouseButtonPressed(MouseButton::Right) && HasSelectedPiece())
		{
			DrawPieceAtSquare(m_SelectedPieceId);
			DeselectPiece();
		}
		if (Input::Get().MouseButtonReleased(MouseButton::Left) && HasSelectedPiece())
		{
			Boxfish::Square square = m_BoardGraphics.ScreenPositionToSquare(Input::Get().MousePosition().xy());
			if (square != Boxfish::INVALID_SQUARE && square != m_SelectedPieceSquare)
			{
				if (m_Board.Move(m_SelectedPieceSquare, square, false))
				{
					DeselectPiece();
				}
			}
			if (HasSelectedPiece())
			{
				DrawPieceAtSquare(m_SelectedPieceId);
				DeselectPiece();
			}
		}
	}

	void ChessGame::SetExitCallback(const std::function<void()>& callback)
	{
		m_OnExit = callback;
	}

	bool ChessGame::HasSelectedPiece() const
	{
		return m_SelectedPieceId >= 0;
	}

	void ChessGame::DeselectPiece()
	{
		m_SelectedPieceId = -1;
		m_SelectedPieceSquare = Boxfish::INVALID_SQUARE;
		m_ValidMoveMarkers.Clear();
	}

	void ChessGame::DrawValidMoves(const std::vector<Boxfish::Move>& moves)
	{
		Color markerColor(10, 10, 10, 150);
		Color captureColor(50, 5, 5, 150);
		for (const Boxfish::Move& move : moves)
		{
			Color c = (move.GetFlags() & (Boxfish::MOVE_CAPTURE | Boxfish::MOVE_EN_PASSANT)) ? captureColor : markerColor;
			m_ValidMoveMarkers.AddMarker(move.GetToSquare(), c, Chess::MarkerType::SmallCircle);
		}
	}

	void ChessGame::DrawPieceAtPosition(int pieceId, const Vector3f& position)
	{
		m_BoardGraphics.OnDrawPieceAtPosition.Emit({ pieceId, position });
	}

	void ChessGame::DrawPieceAtSquare(int pieceId)
	{
		m_BoardGraphics.OnDrawPieceAtSquare.Emit({ pieceId });
	}

	void ChessGame::UpdateAllGraphics()
	{
		m_BoardGraphics.Invalidate();
		m_PreviousMoveMarkers.Invalidate();
		m_ValidMoveMarkers.Invalidate();
		m_Analyzer.Invalidate();

		m_Background = m_SceneData.BoardLayer->GetFactory().Rectangle(m_Window->Width(), m_Window->Height(), Color(120, 120, 120), Transform({ m_Window->Width() / 2.0f, m_Window->Height() / 2.0f, -5.0f }));
	}

	void ChessGame::SetupEventListeners()
	{
		m_Board.OnPieceMoved.AddEventListener([this](Event<Chess::PieceMoved>& e)
		{
			if (e.Data.IsPlayedMove)
			{
				Color markerColor(0, 100, 255, 200);
				m_PreviousMoveMarkers.Clear();
				m_PreviousMoveMarkers.AddMarker(e.Data.From, markerColor);
				m_PreviousMoveMarkers.AddMarker(e.Data.To, markerColor);
			}
		});

		m_WindowResizeListener = m_Window->OnResize().AddScopedEventListener([this](Event<WindowResizeEvent>& e)
		{
			m_Camera.Get().GetComponent<Camera>()->SetProjectionMatrix(Matrix4f::Orthographic(0, m_Window->Width(), 0, m_Window->Height(), -100, 100));
			Vector2f windowSize = { (float)m_Window->Width(), (float)m_Window->Height() };
			float boardSize = std::min(windowSize.x, windowSize.y);
			m_BoardGraphics.SetSize(boardSize);
			m_BoardGraphics.SetPosition({ windowSize / 2.0f, 0.0f });
			UpdateAllGraphics();
		});

		m_MouseMoveListener = Input::Get().OnMouseMoved.AddScopedEventListener([this](Event<MouseMovedEvent>& e)
		{
			if (HasSelectedPiece())
			{
				Vector3f position = { e.Data.x, e.Data.y, 0 };
				DrawPieceAtPosition(m_SelectedPieceId, position);
			}
		});
	}

}
