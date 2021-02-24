#include "Bolt.h"
#include "Boxfish.h"
#include "ChessGame.h"
#include "Players/UCIEnginePlayer.h"
#include "Players/CPWEnginePlayer.h"
#include "AnimationSystem.h"
#include "ChessServerConnection.h"

namespace Chess
{

    AssetHandle<Texture2D> PiecesTexture;
    Matrix3f PieceTextureTransforms[Boxfish::TEAM_MAX][Boxfish::PIECE_MAX];

}

class App : public Application
{
private:
    Chess::SceneData m_ChessGameSceneData;
    std::unique_ptr<Chess::ChessGame> m_ChessGame;
    bool m_ShouldExit = false;

    Boxfish::OpeningBook m_Book;

public:
    void Init() override
    {
        Boxfish::Init();

        Scene& chessScene = SceneManager::Get().AddScene();
        Layer& boardLayer = chessScene.AddLayer();
        Layer& previousMoveLayer = chessScene.AddLayer();
        Layer& validMovesLayer = chessScene.AddLayer();
        Layer& uiLayer = chessScene.AddLayer();
        Layer& piecesLayer = chessScene.AddLayer();

        piecesLayer.Systems().Add<Chess::AnimationSystem>();
        uiLayer.Systems().Add<Chess::AnimationSystem>();

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

        m_ChessGameSceneData.Scene = &chessScene;
        m_ChessGameSceneData.BoardLayer = &boardLayer;
        m_ChessGameSceneData.PreviousMoveLayer = &previousMoveLayer;
        m_ChessGameSceneData.ValidMovesLayer = &validMovesLayer;
        m_ChessGameSceneData.PiecesLayer = &piecesLayer;
        m_ChessGameSceneData.UILayer = &uiLayer;

        m_Book.AppendFromFile("Boxfish.book");
        m_ChessGame = std::make_unique<Chess::ChessGame>(&GetWindow(), m_ChessGameSceneData);
        //Chess::BoxfishPlayer& boxfish = m_ChessGame->AddPlayer<Chess::BoxfishPlayer>(Boxfish::TEAM_BLACK, 128 * 1024 * 1024);
        //boxfish.SetOpeningBook(&m_Book);
        //m_ChessGame->AddPlayer<Chess::BoxfishPlayer>(Boxfish::TEAM_WHITE, 50 * 1024 * 1024);
        //m_ChessGame->AddPlayer<Chess::UCIEnginePlayer>(Boxfish::TEAM_BLACK, Chess::UCILimits{ 4, -1 }, std::vector<std::string>{ "Boxfish-Cli-Old.exe" });
        //m_ChessGame->AddPlayer<Chess::UCIEnginePlayer>(Boxfish::TEAM_BLACK, Chess::UCILimits{ -1, 3000 }, std::vector<std::string>{ "bash", "-c", "./shallowblue" });
        //m_ChessGame->AddPlayer<Chess::UCIEnginePlayer>(Boxfish::TEAM_BLACK, Chess::UCILimits{ -1, 3000 }, std::vector<std::string>{ "Boxfish-Cli-1.0.exe" });
        //m_ChessGame->AddPlayer<Chess::UCIEnginePlayer>(Boxfish::TEAM_WHITE, Chess::UCILimits{ -1, 3000 }, std::vector<std::string>{ "bash", "-c", "./MisterQueen" });
        //m_ChessGame->AddPlayer<Chess::UCIEnginePlayer>(Boxfish::TEAM_BLACK, Chess::UCILimits{ -1, 3000 }, std::vector<std::string>{ "Boxfish-Cli-1.1.exe" });
        //m_ChessGame->AddPlayer<Chess::UCIEnginePlayer>(Boxfish::TEAM_WHITE, Chess::UCILimits{ -1, 3000 }, std::vector<std::string>{ "bash", "-c", "./xiphos-sse" });
        m_ChessGame->AddPlayer<Chess::UCIEnginePlayer>(Boxfish::TEAM_WHITE, Chess::UCILimits{ -1, 3000 }, std::vector<std::string>{ "bash", "-c", "./cinnamon" });
        Chess::UCIEnginePlayer& storm = m_ChessGame->AddPlayer<Chess::UCIEnginePlayer>(Boxfish::TEAM_BLACK, Chess::UCILimits{ -1, 3000 }, std::vector<std::string>{ "C:/Users/jorda/Desktop/dev/C++/Storm/bin/Dist-windows-x86_64/Storm-Cli/Storm-Cli.exe" });
        // storm.SendCommand("setoption name book value Boxfish.book");
        //m_ChessGame->AddPlayer<Chess::UCIEnginePlayer>(Boxfish::TEAM_BLACK, Chess::UCILimits{ -1, 3000 }, std::vector<std::string>{ "bash", "-c", "/mnt/c/users/jorda/desktop/dev/C++/Storm/bin/Dist-linux-x86_64/Storm-Cli/Storm-Cli" });

        // Maia Chess weights file
        //m_ChessGame->AddPlayer<Chess::UCIEnginePlayer>(Boxfish::TEAM_BLACK, Chess::UCILimits{ -1, -1, 1 }, std::vector<std::string>{ "lc0/lc0.exe" });
        //m_ChessGame->AddPlayer<Chess::CPWEnginePlayer>(Boxfish::TEAM_BLACK, std::vector<std::string>{ "cpw1.1.exe" });
        //m_ChessGame->AddPlayer<Chess::UCIEnginePlayer>(Boxfish::TEAM_BLACK, Chess::UCILimits{ -1, 100 }, std::vector<std::string>{ "Stockfish.exe" });
        m_ChessGame->Start();
    }

    void Tick() override
    {

    }

    void Update() override
    {
        if (m_ChessGame)
            m_ChessGame->Update();
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
