#include "BoardAnalyzer.h"
#include "AnimationSystem.h"

namespace Chess
{

	BoardAnalyzer::BoardAnalyzer(Layer* uiLayer, BoardGraphics* board)
		: m_Layer(uiLayer), m_BoardGraphics(board), m_Search(50 * 1024 * 1024, false), m_PonderPosition(), m_SearchMutex(), m_Stop(false), m_SearchThread(), m_CurrentScore(0), m_Running(true),
		m_MovedListener(), m_BoardListener(), m_Flipped(false), m_AnalysisBar()
	{
		Invalidate();
		m_MovedListener = m_BoardGraphics->GetBoard().OnPieceMoved.AddScopedEventListener([this](Event<PieceMoved>& e)
			{
				if (e.Data.IsPlayedMove)
				{
					std::scoped_lock<std::mutex> lock(m_SearchMutex);
					m_PonderPosition = m_BoardGraphics->GetBoard().GetPosition();
					m_Search.Stop();
				}
			});
		m_BoardListener = m_BoardGraphics->GetBoard().OnNewBoard.AddScopedEventListener([this](Event<NewBoard>& e)
			{
				std::scoped_lock<std::mutex> lock(m_SearchMutex);
				m_PonderPosition = e.Data.Board;
				m_Search.Stop();
			});

		m_PonderPosition = Boxfish::CreateStartingPosition();

		BoardAnalyzer* analyzer = this;
		m_SearchThread = std::thread([this, analyzer]()
			{
				while (!m_Stop)
				{
					Boxfish::Position position = m_PonderPosition;
					m_Search.PushPosition(position);
					m_Search.Ponder(position, [position, analyzer](Boxfish::SearchResult result)
						{
							Boxfish::Centipawns score = result.Score;
							if (position.TeamToPlay == Boxfish::TEAM_BLACK)
								score = -score;
							TaskManager::Get().RunOnMainThread([analyzer, score]()
								{
									analyzer->UpdateScore(score);
								});
						});
					std::this_thread::sleep_for(std::chrono::milliseconds(100));
				}
			});
	}

	BoardAnalyzer::~BoardAnalyzer()
	{
		m_Running = false;
		m_Stop = true;
		m_Search.Stop();
		m_SearchThread.join();
	}

	void BoardAnalyzer::Flip()
	{
		m_Flipped = !m_Flipped;
	}

	void BoardAnalyzer::Invalidate()
	{
		float whiteProp = CalculateWhiteProportion(m_CurrentScore);

		if (m_Flipped)
			whiteProp = 1.0 - whiteProp;

		Model barModel;
		barModel.Materials.push_back(AssetManager::Get().Materials().Default(Color(230, 230, 230)));
		barModel.Materials.push_back(AssetManager::Get().Materials().Default(Color(25, 25, 25)));
		barModel.Meshes.push_back({ AssetManager::Get().Meshes().Square(), Matrix4f::Translation(0, -0.5f + whiteProp / 2.0f, 0) * Matrix4f::Scale(1.0f, whiteProp, 1.0f), { (m_Flipped) ? 1 : 0 } });
		barModel.Meshes.push_back({ AssetManager::Get().Meshes().Square(), Matrix4f::Translation(0, +0.5f - (1.0f - whiteProp) / 2.0f, 0) * Matrix4f::Scale(1.0f, 1.0f - whiteProp, 1.0f), { (m_Flipped) ? 0 : 1 } });

		Vector2f boardSize = m_BoardGraphics->GetSize();
		Vector3f boardPosition = m_BoardGraphics->GetPosition();

		m_AnalysisBar = m_Layer->GetFactory().CreateModel(std::move(barModel), Transform({ boardPosition.x - boardSize.x / 2.0f - 40, boardPosition.y, 0 }, Quaternion::Identity(), { 40, boardSize.y - 10, 1 }));
	}

	void BoardAnalyzer::UpdateScore(int score)
	{
		float scale = CalculateWhiteProportion(score);
		if (m_Flipped)
			scale = 1.0f - scale;
		m_AnalysisBar.Get().Assign<BarAnimator>(BarAnimator{ scale, 0.3f });
		m_CurrentScore = score;
	}

	void BoardAnalyzer::Enable()
	{
	}

	void BoardAnalyzer::Disable()
	{
	}

	float BoardAnalyzer::CalculateWhiteProportion(int scoreCentipawns) const
	{
		float scale = (1.0f - std::exp(-0.2 * std::abs(scoreCentipawns) / 100.0f)) / 2.0f;
		if (scoreCentipawns < 0)
			scale *= -1;
		scale += 0.5f;
		return scale;
	}

}
