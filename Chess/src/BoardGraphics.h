#pragma once
#include "Board.h"
#include "PieceGraphics.h"

namespace Chess
{

	class BoardGraphics
	{
	private:
		Layer* m_BoardLayer;
		Layer* m_PiecesLayer;
		Board* m_Board;

		std::unique_ptr<EventBus> m_EventBus;

		std::unordered_map<int, std::unique_ptr<PieceGraphics>> m_Pieces;

		Vector2f m_Size;
		Vector3f m_Position;
		bool m_Flipped;
		ScopedEntityHandle m_BoardEntity;

		ScopedEventListener m_PieceAddedListener;
		ScopedEventListener m_PieceRemovedListener;

	public:
		EventEmitter<DrawPieceAtPosition> OnDrawPieceAtPosition;
		EventEmitter<DrawPieceAtSquare> OnDrawPieceAtSquare;

	public:
		BoardGraphics(Layer* boardLayer, Layer* piecesLayer, Board* board, const Vector2f& size, const Vector3f& position);

		Board& GetBoard() const;

		Vector2f GetSquareSize() const;
		const Vector2f& GetSize() const;
		const Vector3f& GetPosition() const;

		void SetPosition(const Vector3f& position);
		void SetSize(const Vector2f& size);
		void Flip();

		Layer& GetPiecesLayer() const;
		Boxfish::Square ScreenPositionToSquare(const Vector2f& screen) const;
		Vector3f SquareToScreenPosition(const Boxfish::Square& square) const;

		void Invalidate();

	};

}
