#include "BoardAnalyzer.h"

namespace Chess
{

	BoardAnalyzer::BoardAnalyzer(Board* board)
		: m_Board(board), m_Search(50 * 1024 * 1024, false), m_PonderPosition(), m_SearchMutex(), m_Stop(false), m_SearchThread(), m_MovedListener(), m_BoardListener()
	{
		m_MovedListener = m_Board->OnPieceMoved.AddScopedEventListener([this](Event<PieceMoved>& e)
			{
				if (e.Data.IsPlayedMove)
				{
					std::scoped_lock<std::mutex> lock(m_SearchMutex);
					m_PonderPosition = m_Board->GetPosition();
					m_Search.Stop();
				}
			});
		m_BoardListener = m_Board->OnNewBoard.AddScopedEventListener([this](Event<NewBoard>& e)
			{
				std::scoped_lock<std::mutex> lock(m_SearchMutex);
				m_PonderPosition = e.Data.Board;
				m_Search.Stop();
			});

		m_PonderPosition = Boxfish::CreateStartingPosition();

		m_SearchThread = std::thread([this]()
			{
				while (!m_Stop)
				{
					Boxfish::Position position = m_PonderPosition;
					m_Search.SetCurrentPosition(position);
					m_Search.Ponder([position](Boxfish::SearchResult result)
						{
							Boxfish::Centipawns score = result.Score;
							if (position.TeamToPlay == Boxfish::TEAM_BLACK)
								score *= -1;
							std::cout << "Score: " << (score / 100.0f) << std::endl;
						});
				}
			});
	}

	BoardAnalyzer::~BoardAnalyzer()
	{
		m_Stop = true;
		m_Search.Stop();
		m_SearchThread.join();
	}

}
