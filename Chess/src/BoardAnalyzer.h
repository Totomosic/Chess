#pragma once
#include "Boxfish.h"
#include "BoardGraphics.h"
#include <atomic>

namespace Chess
{

	class BoardAnalyzer
	{
	private:
		Layer* m_Layer;
		BoardGraphics* m_BoardGraphics;
		Boxfish::Search m_Search;

		Boxfish::Position m_PonderPosition;
		std::mutex m_SearchMutex;
		std::atomic<bool> m_Stop;
		std::thread m_SearchThread;
		int m_CurrentScore;
		bool m_Running;
		std::atomic<bool> m_Reset;

		ScopedEventListener m_MovedListener;
		ScopedEventListener m_BoardListener;

		bool m_Flipped;
		ScopedEntityHandle m_AnalysisBar;

	public:
		BoardAnalyzer(Layer* uiLayer, BoardGraphics* board);
		~BoardAnalyzer();
		
		void Flip();
		void Invalidate();
		void UpdateScore(int score);
		void Reset();

		void Enable();
		void Disable();

	private:
		float CalculateWhiteProportion(int scoreCentipawns) const;
	};

}
