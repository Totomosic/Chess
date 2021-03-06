#include "PlayerManager.h"

namespace Chess
{

	PlayerManager::PlayerManager(Board* board)
		: m_Board(board), m_CurrentPlayer(nullptr), m_MoveNumber(0), m_Players(), m_MoveListener(), m_BoardListener()
	{
		m_MoveListener = m_Board->OnPieceMoved.AddScopedEventListener([this](Event<PieceMoved>& e)
			{
				if (e.Data.IsPlayedMove && m_Board->IsPlayable())
				{
					GetNextMove(m_Board->GetPosition());
				}
			});
		m_BoardListener = m_Board->OnNewBoard.AddScopedEventListener([this](Event<NewBoard>& e)
			{
				if (m_Board->IsPlayable())
					GetNextMove(e.Data.Board);
			});
	}

	PlayerManager::~PlayerManager()
	{
		if (m_CurrentPlayer)
			m_CurrentPlayer->CancelMove();
	}

	Player& PlayerManager::AddPlayer(std::unique_ptr<Player>&& player)
	{
		Player* p = player.get();
		m_Players.push_back(std::move(player));
		return *p;
	}

	void PlayerManager::GetNextMove(const Boxfish::Position& position)
	{
		if (m_CurrentPlayer)
			m_CurrentPlayer->CancelMove();
		m_CurrentPlayer = nullptr;

		Boxfish::Team teamToPlay = position.TeamToPlay;
		for (const auto& player : m_Players)
		{
			if (player && player->GetTeam() == teamToPlay)
			{
				size_t currentMoveNumber = ++m_MoveNumber;
				m_CurrentPlayer = player.get();
				m_CurrentPlayer->PlayNextMove(m_Board);
				break;
			}
		}
	}

}
