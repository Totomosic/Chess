#pragma once
#include "BoardEvents.h"
#include "GraphicsEvents.h"

namespace Chess
{

	class BoardGraphics;
	extern AssetHandle<Texture2D> PiecesTexture;
	extern Matrix3f PieceTextureTransforms[Boxfish::TEAM_MAX][Boxfish::PIECE_MAX];

	class PieceGraphics
	{
	private:
		BoardGraphics* m_Graphics;
		int m_PieceId;
		Boxfish::Piece m_Type;
		Boxfish::Square m_Square;
		Boxfish::Team m_Team;

		ScopedEntityHandle m_Entity;

		ScopedEventListener m_PositionListener;
		ScopedEventListener m_SquareListener;

		ScopedEventListener m_MovedListener;
		ScopedEventListener m_PromotedListener;

	public:
		PieceGraphics(BoardGraphics* graphics, int pieceId, Boxfish::Piece piece, const Boxfish::Square& square, Boxfish::Team team);

		void DrawAtPosition(const Vector3f& position) const;
		void DrawAtSquare() const;
		void AnimateToPosition(const Vector3f& position) const;
		void AnimateToSquare() const;

		void Invalidate();
	};

}