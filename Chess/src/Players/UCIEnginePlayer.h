#pragma once
#include "../Player.h"
#include "ChildProcess.h"

namespace Chess
{

	struct UCILimits
	{
	public:
		int Depth = -1;
		int MoveTime = -1;
	};

	class UCIEnginePlayer : public Player
	{
	private:
		mutable ChildProcess m_Process;
		mutable Board* m_CurrentBoard;
		UCILimits m_Limits;

	public:
		UCIEnginePlayer(Boxfish::Team team, const UCILimits& limits, const std::vector<std::string>& exectuableArgs);
		~UCIEnginePlayer() override;

		virtual void CancelMove() const override;
		virtual void PlayNextMove(Board* board) const override;
	};

}
