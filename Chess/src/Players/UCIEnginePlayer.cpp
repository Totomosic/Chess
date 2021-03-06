#include "UCIEnginePlayer.h"

namespace Chess
{

	UCIEnginePlayer::UCIEnginePlayer(Boxfish::Team team, const UCILimits& limits, const std::vector<std::string>& executableArgs) : Player(team),
		m_Process(executableArgs), m_CurrentBoard(nullptr), m_Limits(limits)
	{
		m_Process.OnMessageReceived.AddEventListener([this](Event<ChildProcessMessage>& e)
		{
			if (m_CurrentBoard)
			{
				for (std::string& message : blt::split(e.Data.Message, '\n'))
				{
					if (!message.empty())
					{
						std::cout << message << std::endl;
						std::string token = "bestmove ";
						if (message.substr(0, token.size()) == token)
						{
							std::string move = message.substr(token.size());
							size_t space = move.find_first_of(' ');
							if (space != std::string::npos)
								move = move.substr(0, space);
							while (move.back() == '\r')
								move.pop_back();
							Boxfish::Move mv = Boxfish::UCI::CreateMoveFromString(m_CurrentBoard->GetPosition(), move);
							m_CurrentBoard->Move(mv);
						}
					}
				}
			}
		});

		// m_Process.SendCommand("setoption name Skill Level value 10");
	}

	UCIEnginePlayer::~UCIEnginePlayer()
	{
		CancelMove();
	}

	void UCIEnginePlayer::SendCommand(const std::string& command) const
	{
		m_Process.SendCommand(command);
	}

	void UCIEnginePlayer::CancelMove() const
	{
		m_Process.SendCommand("stop");
		m_CurrentBoard = nullptr;
	}

	void UCIEnginePlayer::PlayNextMove(Board* board) const
	{
		m_CurrentBoard = board;
		m_Process.SendCommand(board->GetUCIString());
		if (m_Limits.MoveTime > 0)
			m_Process.SendCommand("go movetime " + std::to_string(m_Limits.MoveTime));
		else if (m_Limits.Depth > 0)
			m_Process.SendCommand("go depth " + std::to_string(m_Limits.Depth));
		else
			m_Process.SendCommand("go nodes " + std::to_string(m_Limits.Nodes));
	}

}
