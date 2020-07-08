#include "BoardMarkers.h"

namespace Chess
{

	BoardMarkers::BoardMarkers(Layer* layer, const BoardGraphics* graphics)
		: m_Layer(layer), m_Graphics(graphics), m_Markers()
	{
	}

	void BoardMarkers::AddMarker(const Boxfish::Square& square, const Color& color)
	{
		Vector2f size = m_Graphics->GetSquareSize();
		Vector3f position = m_Graphics->SquareToScreenPosition(square);
		m_Markers[Boxfish::BitBoard::SquareToBitIndex(square)] = { m_Layer->GetFactory().Rectangle(size.x, size.y, color, { position }), color };
	}

	void BoardMarkers::RemoveMarker(const Boxfish::Square& square)
	{
		auto it = m_Markers.find(Boxfish::BitBoard::SquareToBitIndex(square));
		if (it != m_Markers.end())
			m_Markers.erase(it);
	}

	void BoardMarkers::Clear()
	{
		m_Markers.clear();
	}

	void BoardMarkers::Invalidate()
	{
		for (auto& pair : m_Markers)
		{
			AddMarker(Boxfish::BitBoard::BitIndexToSquare(pair.first), pair.second.MarkerColor);
		}
	}

}
