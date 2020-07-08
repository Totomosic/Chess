#include "PieceGraphics.h"
#include "BoardGraphics.h"

namespace Chess
{

	PieceGraphics::PieceGraphics(BoardGraphics* graphics, int pieceId, Boxfish::Piece piece, const Boxfish::Square& square, Boxfish::Team team)
		: m_Graphics(graphics), m_PieceId(pieceId), m_Type(piece), m_Square(square), m_Team(team), m_Entity(),
		m_PositionListener(), m_SquareListener(), m_MovedListener(), m_PromotedListener()
	{
		m_PositionListener = m_Graphics->OnDrawPieceAtPosition.AddScopedEventListener([this](Event<DrawPieceAtPosition>& e)
			{
				if (e.Data.PieceId == m_PieceId)
				{
					DrawAtPosition(e.Data.Position);
				}
			});

		m_SquareListener = m_Graphics->OnDrawPieceAtSquare.AddScopedEventListener([this](Event<DrawPieceAtSquare>& e)
			{
				if (e.Data.PieceId == m_PieceId)
				{
					DrawAtSquare();
				}
			});

		m_MovedListener = m_Graphics->GetBoard().OnPieceMoved.AddScopedEventListener([this](Event<PieceMoved>& e)
			{
				if (e.Data.PieceId == m_PieceId)
				{
					m_Square = e.Data.To;
					DrawAtSquare();
				}
			});

		m_PromotedListener = m_Graphics->GetBoard().OnPiecePromoted.AddScopedEventListener([this](Event<PiecePromoted>& e)
			{
				if (e.Data.PieceId == m_PieceId)
				{
					m_Type = e.Data.Promotion;
					Invalidate();
				}
			});

		Invalidate();
	}

	void PieceGraphics::DrawAtPosition(const Vector3f& position) const
	{
		m_Entity.Get().GetTransform()->SetLocalPosition(position);
	}

	void PieceGraphics::DrawAtSquare() const
	{
		DrawAtPosition(m_Graphics->SquareToScreenPosition(m_Square));
	}

	void PieceGraphics::Invalidate()
	{
		auto material = AssetManager::Get().Materials().Texture(PiecesTexture);
		material->SetIsTransparent(true);
		material->LinkTextureTransform(PieceTextureTransforms[m_Team][m_Type]);
		Vector2f squareSize = m_Graphics->GetSquareSize();
		Vector3f position = m_Graphics->SquareToScreenPosition(m_Square);
		m_Entity = m_Graphics->GetPiecesLayer().GetFactory().Rectangle(squareSize.x, squareSize.y, std::move(material), { position });
	}

}
