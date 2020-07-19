#pragma once
#include "Bolt.h"

namespace Chess
{

	struct JoinableGame
	{
	public:
		int GameId;
		std::string HostUsername;
	};

	class ChessServerConnection
	{
	private:
		mutable TCPsocket m_Socket;
		std::string m_Username;
		bool m_IsConnected;

	public:
		ChessServerConnection(const SocketAddress& serverAddress, const std::string& username);
		~ChessServerConnection();

		bool IsConnected() const;

		bool StartJoinableGame(int* outGameId) const;
		bool FinishGame(int gameId) const;
		std::vector<JoinableGame> ListJoinableGames() const;

	private:
		std::string EncodeMessage(const std::vector<std::string>& message) const;
		std::vector<std::string> DecodeMessage(const std::string& message) const;
		bool SendServerMessage(const std::string& message, std::vector<std::string>* outResponse) const;
	};

}
