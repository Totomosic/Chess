#pragma once
#include "../Player.h"
#include "Boxfish.h"

namespace Chess
{

	class BoxfishPlayer : public Player
	{
	private:
		mutable Boxfish::Search m_Search;
		mutable std::atomic<bool> m_Running;
		mutable std::thread m_SearchThread;

	public:
		BoxfishPlayer(Boxfish::Team team, size_t ttSize = Boxfish::TranspositionTable::TABLE_SIZE);
		~BoxfishPlayer() override;

		virtual void CancelMove() const override;
		virtual void PlayNextMove(Board* board) const override;
	};

}
