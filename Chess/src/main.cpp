#include "Bolt.h"
#include "Boxfish.h"

#include "Board.h"
#include "BoardGraphics.h"
#include "BoardMarkers.h"

namespace Chess
{

    AssetHandle<Texture2D> PiecesTexture;
    Matrix3f PieceTextureTransforms[Boxfish::TEAM_MAX][Boxfish::PIECE_MAX];

}

class App : public Application
{
private:
    std::unique_ptr<Chess::Board> m_Board;
    std::unique_ptr<Chess::BoardGraphics> m_BoardGraphics;
    std::unique_ptr<Chess::BoardMarkers> m_PreviousMoveMarkers;
    std::unique_ptr<Chess::BoardMarkers> m_ValidMoveMarkers;

    int m_SelectedPieceId = -1;
    Boxfish::Square m_SelectedPieceSquare = Boxfish::INVALID_SQUARE;

public:
    void Init() override
    {
        ::Boxfish::Init();
        GetWindow().SetClearColor(Color::Black);

        Scene& scene = SceneManager::Get().AddScene();
        Layer& boardLayer = scene.AddLayer();
        Layer& previousMoveLayer = scene.AddLayer();
        Layer& validMovesLayer = scene.AddLayer();
        Layer& piecesLayer = scene.AddLayer();

        EntityHandle camera = scene.GetFactory().Camera(Matrix4f::Orthographic(0, GetWindow().Width(), 0, GetWindow().Height(), -100, 100));
        boardLayer.SetActiveCamera(camera);
        previousMoveLayer.SetActiveCamera(camera);
        validMovesLayer.SetActiveCamera(camera);
        piecesLayer.SetActiveCamera(camera);

        Chess::PiecesTexture = AssetManager::Get().LoadAsset<Texture2D>("res/Pieces.bltasset");
        Chess::PieceTextureTransforms[Boxfish::TEAM_WHITE][Boxfish::PIECE_PAWN] = Matrix3f::Translation(5.0f / 6.0f, 0.5f) * Matrix3f::Scale(1.0f / 6.0f, 0.5f, 1.0f);
        Chess::PieceTextureTransforms[Boxfish::TEAM_WHITE][Boxfish::PIECE_KNIGHT] = Matrix3f::Translation(3.0f / 6.0f, 0.5f) * Matrix3f::Scale(1.0f / 6.0f, 0.5f, 1.0f);
        Chess::PieceTextureTransforms[Boxfish::TEAM_WHITE][Boxfish::PIECE_BISHOP] = Matrix3f::Translation(2.0f / 6.0f, 0.5f) * Matrix3f::Scale(1.0f / 6.0f, 0.5f, 1.0f);
        Chess::PieceTextureTransforms[Boxfish::TEAM_WHITE][Boxfish::PIECE_ROOK] = Matrix3f::Translation(4.0f / 6.0f, 0.5f) * Matrix3f::Scale(1.0f / 6.0f, 0.5f, 1.0f);
        Chess::PieceTextureTransforms[Boxfish::TEAM_WHITE][Boxfish::PIECE_QUEEN] = Matrix3f::Translation(1.0f / 6.0f, 0.5f) * Matrix3f::Scale(1.0f / 6.0f, 0.5f, 1.0f);
        Chess::PieceTextureTransforms[Boxfish::TEAM_WHITE][Boxfish::PIECE_KING] = Matrix3f::Translation(0.0f / 6.0f, 0.5f) * Matrix3f::Scale(1.0f / 6.0f, 0.5f, 1.0f);

        Chess::PieceTextureTransforms[Boxfish::TEAM_BLACK][Boxfish::PIECE_PAWN] = Matrix3f::Translation(5.0f / 6.0f, 0.0f) * Matrix3f::Scale(1.0f / 6.0f, 0.5f, 1.0f);
        Chess::PieceTextureTransforms[Boxfish::TEAM_BLACK][Boxfish::PIECE_KNIGHT] = Matrix3f::Translation(3.0f / 6.0f, 0.0f) * Matrix3f::Scale(1.0f / 6.0f, 0.5f, 1.0f);
        Chess::PieceTextureTransforms[Boxfish::TEAM_BLACK][Boxfish::PIECE_BISHOP] = Matrix3f::Translation(2.0f / 6.0f, 0.0f) * Matrix3f::Scale(1.0f / 6.0f, 0.5f, 1.0f);
        Chess::PieceTextureTransforms[Boxfish::TEAM_BLACK][Boxfish::PIECE_ROOK] = Matrix3f::Translation(4.0f / 6.0f, 0.0f) * Matrix3f::Scale(1.0f / 6.0f, 0.5f, 1.0f);
        Chess::PieceTextureTransforms[Boxfish::TEAM_BLACK][Boxfish::PIECE_QUEEN] = Matrix3f::Translation(1.0f / 6.0f, 0.0f) * Matrix3f::Scale(1.0f / 6.0f, 0.5f, 1.0f);
        Chess::PieceTextureTransforms[Boxfish::TEAM_BLACK][Boxfish::PIECE_KING] = Matrix3f::Translation(0.0f / 6.0f, 0.0f) * Matrix3f::Scale(1.0f / 6.0f, 0.5f, 1.0f);

        Vector2f windowSize = { (float)GetWindow().Width(), (float)GetWindow().Height() };
        float boardSize = std::min(windowSize.x, windowSize.y);

        m_Board = std::make_unique<Chess::Board>();
        m_BoardGraphics = std::make_unique<Chess::BoardGraphics>(&boardLayer, &piecesLayer, m_Board.get(), Vector2f{ boardSize, boardSize }, Vector3f{ windowSize / 2.0f, 0.0f });
        m_PreviousMoveMarkers = std::make_unique<Chess::BoardMarkers>(&previousMoveLayer, m_BoardGraphics.get());
        m_ValidMoveMarkers = std::make_unique<Chess::BoardMarkers>(&validMovesLayer, m_BoardGraphics.get());

        m_Board->SetStartingPosition();

        m_Board->OnPieceMoved.AddEventListener([this](Event<Chess::PieceMoved>& e)
        {
            if (e.Data.IsPlayedMove)
            {
                Color markerColor(0, 100, 255, 200);
                m_PreviousMoveMarkers->Clear();
                m_PreviousMoveMarkers->AddMarker(e.Data.From, markerColor);
                m_PreviousMoveMarkers->AddMarker(e.Data.To, markerColor);
            }
        });

        GetWindow().OnResize().AddEventListener([camera, this](Event<WindowResizeEvent>& e)
            {
                camera.GetComponent<Camera>()->SetProjectionMatrix(Matrix4f::Orthographic(0, GetWindow().Width(), 0, GetWindow().Height(), -100, 100));
                Vector2f windowSize = { (float)GetWindow().Width(), (float)GetWindow().Height() };
                float boardSize = std::min(windowSize.x, windowSize.y);
                m_BoardGraphics->SetSize(boardSize);
                m_BoardGraphics->SetPosition({ windowSize / 2.0f, 0.0f });
                RedrawGraphics();
            });

        Input::Get().OnMouseMoved.AddEventListener([this](Event<MouseMovedEvent>& e)
            {
                if (HasSelectedPiece())
                {
                    Vector3f position = { e.Data.x, e.Data.y, 0 };
                    DrawPieceAtPosition(m_SelectedPieceId, position);
                }
            });
    }

    void RedrawGraphics()
    {
        m_BoardGraphics->Invalidate();
        m_PreviousMoveMarkers->Invalidate();
        m_ValidMoveMarkers->Invalidate();
    }

    bool HasSelectedPiece() const
    {
        return m_SelectedPieceId >= 0;
    }

    void DrawPieceAtPosition(int pieceId, const Vector3f& position) const
    {
        m_BoardGraphics->OnDrawPieceAtPosition.Emit({ pieceId, position });
    }

    void DrawPieceAtSquare(int pieceId) const
    {
        m_BoardGraphics->OnDrawPieceAtSquare.Emit({ pieceId });
    }

    void DrawValidMoves(const std::vector<Boxfish::Move>& moves)
    {
        Color markerColor(10, 10, 10, 150);
        Color captureColor(50, 5, 5, 150);
        for (const Boxfish::Move& move : moves)
        {
            Color c = (move.GetFlags() & (Boxfish::MOVE_CAPTURE | Boxfish::MOVE_EN_PASSANT)) ? captureColor : markerColor;
            m_ValidMoveMarkers->AddMarker(move.GetToSquare(), c, Chess::MarkerType::SmallCircle);
        }
    }

    void DeselectPiece()
    {
        m_SelectedPieceId = -1;
        m_SelectedPieceSquare = Boxfish::INVALID_SQUARE;
        m_ValidMoveMarkers->Clear();
    }

    void Tick() override
    {

    }

    void Update() override
    {
        if (Input::Get().KeyPressed(Keycode::Left))
        {
            m_Board->PopMove();
        }
        if (Input::Get().KeyPressed(Keycode::F))
        {
            std::cout << m_Board->GetPosition() << std::endl;
            std::cout << "Fen: " << Boxfish::GetFENFromPosition(m_Board->GetPosition()) << std::endl;
            std::cout << "Hash: " << std::hex << m_Board->GetPosition().Hash.Hash << std::dec << std::endl;

            m_BoardGraphics->Flip();
            RedrawGraphics();
        }

        if (Input::Get().MouseButtonPressed(MouseButton::Left) && !HasSelectedPiece())
        {
            Boxfish::Square square = m_BoardGraphics->ScreenPositionToSquare(Input::Get().MousePosition().xy());
            if (square != Boxfish::INVALID_SQUARE)
            {
                int pieceId = m_Board->GetPieceIdAt(square);
                if (pieceId >= 0)
                {
                    m_SelectedPieceId = pieceId;
                    m_SelectedPieceSquare = square;
                    DrawPieceAtPosition(m_SelectedPieceId, Input::Get().MousePosition());
                    DrawValidMoves(m_Board->GetLegalMovesFor(square));
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
            Boxfish::Square square = m_BoardGraphics->ScreenPositionToSquare(Input::Get().MousePosition().xy());
            if (square != Boxfish::INVALID_SQUARE && square != m_SelectedPieceSquare)
            {
                if (m_Board->Move(m_SelectedPieceSquare, square, true))
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

    void Render() override
    {
        Graphics::Get().RenderScene();
    }
};

int main(int argc, char** argv)
{
    EngineCreateInfo engine;
    engine.EnableImGui = true;
    Engine e(engine);
    e.SetApplication<App>();
    e.Run();
    return 0;
}
