#include "BoxfishPlayer.h"

namespace Chess
{

    BoxfishPlayer::BoxfishPlayer(Boxfish::Team team, size_t ttSize) : Player(team),
        m_Search(ttSize, true), m_Running(false), m_SearchThread()
    {
        Boxfish::BoxfishSettings settings;
        settings.SkillLevel = 20;
        m_Search.SetSettings(settings);
    }

    BoxfishPlayer::~BoxfishPlayer()
    {
        CancelMove();
    }

    void BoxfishPlayer::CancelMove() const
    {
        if (m_Running)
        {
            m_Running = false;
            m_Search.Stop();
        }
        if (m_SearchThread.joinable())
            m_SearchThread.join();
    }

    void BoxfishPlayer::PlayNextMove(Board* board) const
    {
        m_Search.PushPosition(board->GetPosition());
        CancelMove();
        m_Running = true;
        const BoxfishPlayer* playerPtr = this;
        m_SearchThread = std::thread([playerPtr, board]()
            {
                Boxfish::SearchLimits limits;
                limits.Milliseconds = 3000;
                Boxfish::Move bestMove = playerPtr->m_Search.SearchBestMove(board->GetPosition(), limits);
                if (playerPtr->m_Running)
                {
                    TaskManager::Get().RunOnMainThread([playerPtr, board, bestMove]()
                        {
                            board->Move(bestMove);
                            playerPtr->m_Search.PushPosition(board->GetPosition());
                        });
                    playerPtr->m_Running = false;
                }
            });
    }

}
