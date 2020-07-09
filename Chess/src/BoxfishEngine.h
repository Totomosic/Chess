#pragma once
#include "Boxfish.h"
#include "Bolt.h"

namespace Chess
{

	class BoxfishEngine
	{
	private:
		Boxfish::Search m_Search;

	public:
		BoxfishEngine(size_t ttSize = Boxfish::TranspositionTable::TABLE_SIZE);

		Task<Boxfish::Move> GetBestMove(int milliseconds);
		void SetCurrentPosition(const Boxfish::Position& position);
		void Reset();

	};

}
