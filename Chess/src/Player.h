#pragma once
#include "Board.h"

namespace Chess
{

	class Player
	{
	protected:
		Boxfish::Team m_Team;

	public:
		Player(Boxfish::Team team);
		virtual ~Player() = default;

		Boxfish::Team GetTeam() const;
		virtual void CancelMove() const = 0;
		virtual void PlayNextMove(Board* board) const = 0;
	};

}
