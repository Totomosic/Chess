#include "BoxfishEngine.h"

namespace Chess
{

    BoxfishEngine::BoxfishEngine(size_t ttSize)
        : m_Search(ttSize, false)
    {
    }

    Task<Boxfish::Move> BoxfishEngine::GetBestMove(const Boxfish::Position& position, int milliseconds)
    {
        return TaskManager::Get().Run([this, milliseconds, position]()
            {
                Boxfish::SearchLimits limits;
                limits.Milliseconds = milliseconds;
                return m_Search.SearchBestMove(position, limits);
            });
    }

}
