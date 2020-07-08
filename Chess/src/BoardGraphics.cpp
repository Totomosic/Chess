#include "BoardGraphics.h"

namespace Chess
{

	BoardGraphics::BoardGraphics(Layer* boardLayer, Layer* piecesLayer, Board* board, const Vector2f& size, const Vector3f& position)
		: m_BoardLayer(boardLayer), m_PiecesLayer(piecesLayer), m_Board(board), m_EventBus(std::make_unique<EventBus>()), m_Pieces(), m_Size(size), m_Position(position), m_Flipped(false), m_BoardEntity(), m_PieceAddedListener(), m_PieceRemovedListener(),
		OnDrawPieceAtPosition(m_EventBus->GetEmitter<DrawPieceAtPosition>()),
		OnDrawPieceAtSquare(m_EventBus->GetEmitter<DrawPieceAtSquare>())
	{
		m_EventBus->SetImmediateMode(true);
		BoardGraphics* graphicsPointer = this;
		m_PieceAddedListener = m_Board->OnPieceAdded.AddScopedEventListener([this, graphicsPointer](Event<PieceAdded>& e)
			{
				m_Pieces[e.Data.PieceId] = std::make_unique<PieceGraphics>(graphicsPointer, e.Data.PieceId, e.Data.Type, e.Data.Square, e.Data.Team);
			});
		m_PieceRemovedListener = m_Board->OnPieceRemoved.AddScopedEventListener([this](Event<PieceRemoved>& e)
			{
				m_Pieces.erase(e.Data.PieceId);
			});

		Invalidate();
	}

	Board& BoardGraphics::GetBoard() const
	{
		return *m_Board;
	}

	Vector2f BoardGraphics::GetSquareSize() const
	{
		return GetSize() / Vector2f{ Boxfish::FILE_MAX, Boxfish::RANK_MAX };
	}

	const Vector2f& BoardGraphics::GetSize() const
	{
		return m_Size;
	}

	const Vector3f& BoardGraphics::GetPosition() const
	{
		return m_Position;
	}

	void BoardGraphics::SetPosition(const Vector3f& position)
	{
		m_Position = position;
	}

	void BoardGraphics::SetSize(const Vector2f& size)
	{
		m_Size = size;
	}

	void BoardGraphics::Flip()
	{
		m_Flipped = !m_Flipped;
	}

	Layer& BoardGraphics::GetPiecesLayer() const
	{
		return *m_PiecesLayer;
	}

	Boxfish::Square BoardGraphics::ScreenPositionToSquare(const Vector2f& screen) const
	{
		Vector2f offset = GetSize() / 2.0f;
		Vector2f squareSize = GetSquareSize();

		float x = screen.x - m_Position.x + offset.x;
		float y = screen.y - m_Position.y + offset.y;

		Boxfish::File file = (Boxfish::File)(x / squareSize.x);
		Boxfish::Rank rank = (Boxfish::Rank)(y / squareSize.y);
		if (m_Flipped)
		{
			file = (Boxfish::File)(Boxfish::FILE_MAX - file - 1);
			rank = (Boxfish::Rank)(Boxfish::RANK_MAX - rank - 1);
		}

		if (file < 0 || file >= Boxfish::FILE_MAX || rank < 0 || rank >= Boxfish::RANK_MAX)
			return Boxfish::INVALID_SQUARE;

		return { file, rank };
	}

	Vector3f BoardGraphics::SquareToScreenPosition(const Boxfish::Square& square) const
	{
		Vector2f offset = GetSize() / 2.0f;
		Vector2f squareSize = GetSquareSize();
		Boxfish::File file = (m_Flipped) ? (Boxfish::File)(Boxfish::FILE_MAX - square.File - 1) : square.File;
		Boxfish::Rank rank = (m_Flipped) ? (Boxfish::Rank)(Boxfish::RANK_MAX - square.Rank - 1) : square.Rank;
		float x = file * squareSize.x + squareSize.x / 2.0f - offset.x + m_Position.x;
		float y = rank * squareSize.y + squareSize.y / 2.0f - offset.y + m_Position.y;
		return { x, y, m_Position.z };
	}

	void BoardGraphics::Invalidate()
	{
		Color lightSquareColor(232, 235, 239);
		Color darkSquareColor(20, 50, 100);
		Vector2f squareSize = GetSquareSize();

		Model boardModel;
		boardModel.Materials.push_back(AssetManager::Get().Materials().Default(lightSquareColor));
		boardModel.Materials.push_back(AssetManager::Get().Materials().Default(darkSquareColor));
		AssetHandle<Mesh> squareMesh = AssetManager::Get().Meshes().Square();

		Vector2f offset = GetSize() / 2.0f;

		for (Boxfish::File file = Boxfish::FILE_A; file < Boxfish::FILE_MAX; file++)
		{
			for (Boxfish::Rank rank = Boxfish::RANK_1; rank < Boxfish::RANK_MAX; rank++)
			{
				bool isLight = (file + rank) % 2 == 1;
				float x = file * squareSize.x + squareSize.x / 2.0f - offset.x;
				float y = rank * squareSize.y + squareSize.y / 2.0f - offset.y;
				boardModel.Meshes.push_back({ squareMesh, Matrix4f::Translation(x, y, 0) * Matrix4f::Scale(squareSize.x, squareSize.y, 1.0f), { (isLight) ? 0 : 1 } });
			}
		}

		m_BoardEntity = m_BoardLayer->GetFactory().CreateModel(std::move(boardModel), { m_Position });

		for (auto& pair : m_Pieces)
		{
			pair.second->Invalidate();
		}
	}

}
