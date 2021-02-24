#include "CPWEnginePlayer.h"
#include "Board.h"

namespace Chess
{

	CPWEnginePlayer::CPWEnginePlayer(Boxfish::Team team, const std::vector<std::string>& exectuableArgs) : Player(team),
		m_Process(exectuableArgs), m_CurrentBoard(nullptr)
	{
		m_Process.OnMessageReceived.AddEventListener([this](Event<ChildProcessMessage>& e)
		{
			static std::string fullMessage = "";
			if (m_CurrentBoard)
			{
				fullMessage += e.Data.Message;
				if (!fullMessage.empty() && fullMessage.back() == '\n')
				{
					for (std::string& message : blt::split(fullMessage, '\n'))
					{
						if (!message.empty())
						{
							std::cout << message << std::endl;
							std::string token = "CPW: ";
							if (message.substr(0, token.size()) == token)
							{
								std::string move = message.substr(token.size());
								if (move.back() == '\r')
									move.pop_back();
								Boxfish::Move mv = Boxfish::UCI::CreateMoveFromString(m_CurrentBoard->GetPosition(), move);
								m_CurrentBoard->Move(mv);
							}
						}
					}
					fullMessage = "";
				}
			}
		});

		m_Process.SendCommand("st 3");
		m_Process.SendCommand("new");
	}

	CPWEnginePlayer::~CPWEnginePlayer()
	{
		CancelMove();
	}

	void CPWEnginePlayer::CancelMove() const
	{
		m_CurrentBoard = nullptr;
	}

	void CPWEnginePlayer::PlayNextMove(Board* board) const
	{
		m_CurrentBoard = board;
		if (m_CurrentBoard->GetMoveHistory().size() > 0)
		{
			Board::MoveInfo lastMove = m_CurrentBoard->GetMoveHistory().back();
			m_Process.SendCommand(Boxfish::UCI::FormatMove(lastMove.Move));
		}
		else
		{
			m_Process.SendCommand("go");
		}
	}

}
