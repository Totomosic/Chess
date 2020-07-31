#include "BoxfishPlayer.h"

namespace Chess
{

    BoxfishPlayer::BoxfishPlayer(Boxfish::Team team, size_t ttSize) : Player(team),
        m_Search(ttSize, true), m_Running(false), m_SearchThread()
    {
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
        m_Search.SetCurrentPosition(board->GetPosition());
        m_Search.GetHistory().Push(board->GetPosition());
        CancelMove();
        m_Running = true;
        const BoxfishPlayer* playerPtr = this;
        m_SearchThread = std::thread([playerPtr, board]()
            {
                Boxfish::SearchLimits limits;
                limits.Milliseconds = 3000;
                playerPtr->m_Search.SetLimits(limits);
                Boxfish::Move bestMove = playerPtr->m_Search.Go(Boxfish::MAX_PLY);
                if (playerPtr->m_Running)
                {
                    TaskManager::Get().RunOnMainThread([playerPtr, board, bestMove]()
                        {
                            board->Move(bestMove);
                            playerPtr->m_Search.GetHistory().Push(board->GetPosition());
                        });
                    playerPtr->m_Running = false;
                }
            });
    }

}
