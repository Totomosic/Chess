#pragma once
#include "../Player.h"
#include "ChildProcess.h"

namespace Chess
{

	class CPWEnginePlayer : public Player
	{
	private:
		mutable ChildProcess m_Process;
		mutable Board* m_CurrentBoard;

	public:
		CPWEnginePlayer(Boxfish::Team team, const std::vector<std::string>& exectuableArgs);
		~CPWEnginePlayer() override;

		virtual void CancelMove() const override;
		virtual void PlayNextMove(Board* board) const override;
	};

}
