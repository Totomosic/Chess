#pragma once
#include "Board.h"
#include "Player.h"

namespace Chess
{

	class PlayerManager
	{
	private:
		Board* m_Board;
		Player* m_CurrentPlayer;
		size_t m_MoveNumber;
		std::vector<std::unique_ptr<Player>> m_Players;

		ScopedEventListener m_MoveListener;
		ScopedEventListener m_BoardListener;

	public:
		PlayerManager(Board* board);
		~PlayerManager();

		Player& AddPlayer(std::unique_ptr<Player>&& player);

		template<typename T, typename ... Args>
		T& AddPlayer(Args&& ... args)
		{
			return (T&)AddPlayer(std::make_unique<T>(std::forward<Args>(args)...));
		}

	private:
		void GetNextMove(const Boxfish::Position& position);
	};

}
