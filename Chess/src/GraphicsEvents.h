#pragma once
#include "Bolt.h"

namespace Chess
{

	struct DrawPieceAtPosition
	{
	public:
		int PieceId;
		Vector3f Position;
	};

	struct DrawPieceAtSquare
	{
	public:
		int PieceId;
	};

}
