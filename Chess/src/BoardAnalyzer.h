#pragma once
#include "Boxfish.h"
#include "Board.h"
#include <atomic>

namespace Chess
{

	class BoardAnalyzer
	{
	private:
		Board* m_Board;
		Boxfish::Search m_Search;

		Boxfish::Position m_PonderPosition;
		std::mutex m_SearchMutex;
		std::atomic<bool> m_Stop;
		std::thread m_SearchThread;

		ScopedEventListener m_MovedListener;
		ScopedEventListener m_BoardListener;

	public:
		BoardAnalyzer(Board* board);
		~BoardAnalyzer();
	};

}
