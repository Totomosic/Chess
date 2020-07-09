#include "BoxfishEngine.h"

namespace Chess
{

    BoxfishEngine::BoxfishEngine(size_t ttSize)
        : m_Search(ttSize, false)
    {
    }

    Task<Boxfish::Move> BoxfishEngine::GetBestMove(int milliseconds)
    {
        return TaskManager::Get().Run([this, milliseconds]()
            {
                Boxfish::SearchLimits limits;
                limits.Milliseconds = milliseconds;
                m_Search.SetLimits(limits);
                return m_Search.Go(50);
            });
    }

    void BoxfishEngine::SetCurrentPosition(const Boxfish::Position& position)
    {
        m_Search.SetCurrentPosition(position);
        m_Search.GetHistory().Push(position);
    }

    void BoxfishEngine::Reset()
    {
        m_Search.Reset();
    }

}
