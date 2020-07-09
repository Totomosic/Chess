#pragma once
#include "Boxfish.h"

namespace Chess
{

	struct PieceMoved
	{
	public:
		int PieceId;
		Boxfish::Square From;
		Boxfish::Square To;
		bool Animate;
		// Rook move during castle = false
		bool IsPlayedMove;
	};

	struct PieceAdded
	{
	public:
		int PieceId;
		Boxfish::Piece Type;
		Boxfish::Team Team;
		Boxfish::Square Square;
	};

	struct PieceRemoved
	{
	public:
		int PieceId;
	};

	struct PiecePromoted
	{
	public:
		int PieceId;
		Boxfish::Piece Promotion;
	};

	struct NewBoard
	{
	public:
		Boxfish::Position Board;
	};

}
