#include "ChessServerConnection.h"

namespace Chess
{

    ChessServerConnection::ChessServerConnection(const SocketAddress& serverAddress, const std::string& username)
        : m_Socket(), m_IsConnected(false), m_Username(username)
    {
        m_Socket.Connect(serverAddress);
        m_IsConnected = SendServerMessage(EncodeMessage({ "CONNECT", m_Username }), nullptr);
    }

    ChessServerConnection::~ChessServerConnection()
    {
        if (IsConnected())
            SendServerMessage(EncodeMessage({ "DISCONNECT" }), nullptr);
    }

    bool ChessServerConnection::IsConnected() const
    {
        return m_IsConnected;
    }

    bool ChessServerConnection::StartJoinableGame(int* outGameId) const
    {
        BLT_ASSERT(IsConnected(), "Must be connected");
        std::vector<std::string> response;
        if (SendServerMessage("CREATE_GAME", &response))
        {
            if (outGameId)
                *outGameId = std::stoi(response[0]);
            return true;
        }
        return false;
    }

    bool ChessServerConnection::FinishGame(int gameId) const
    {
        BLT_ASSERT(IsConnected(), "Must be connected");
        return SendServerMessage(EncodeMessage({ "FINISH_GAME", std::to_string(gameId) }), nullptr);
    }

    std::vector<JoinableGame> ChessServerConnection::ListJoinableGames() const
    {
        BLT_ASSERT(IsConnected(), "Must be connected");
        std::vector<JoinableGame> result;
        std::vector<std::string> response;
        if (SendServerMessage("LIST_GAMES", &response))
        {
            for (const std::string& game : response)
            {
                size_t space = game.find_first_of(' ');
                if (space != std::string::npos)
                {
                    int gameId = std::stoi(game.substr(0, space));
                    std::string username = game.substr(space + 1);
                    result.push_back({ gameId, username });
                }
            }
        }
        return result;
    }

    std::string ChessServerConnection::EncodeMessage(const std::vector<std::string>& message) const
    {
        if (message.size() > 0)
        {
            std::string result = message[0];
            for (int i = 1; i < message.size(); i++)
            {
                result += '\n' + message[i];
            }
            return result;
        }
        return "";
    }

    std::vector<std::string> ChessServerConnection::DecodeMessage(const std::string& message) const
    {
        return blt::split(message, '\n');
    }

    bool ChessServerConnection::SendServerMessage(const std::string& message, std::vector<std::string>* outResponse) const
    {
        m_Socket.Send(message.data(), message.length());
        char buffer[1024];
        int received = m_Socket.Recv(buffer, sizeof(buffer));
        if (received > 0)
        {
            buffer[received] = '\0';
            std::vector<std::string> response = DecodeMessage(buffer);
            if (outResponse)
            {
                for (int i = 1; i < response.size(); i++)
                    outResponse->push_back(response[i]);
            }
            return response[0] == "OK";
        }
        return false;
    }

}
