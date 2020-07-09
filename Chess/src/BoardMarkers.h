#pragma once
#include "BoardGraphics.h"

namespace Chess
{

	enum class MarkerType
	{
		Square,
		SmallCircle
	};

	class BoardMarkers
	{
	private:
		struct Marker
		{
		public:
			ScopedEntityHandle Entity;
			Color MarkerColor;
			MarkerType Type;
		};

	private:
		Layer* m_Layer;
		const BoardGraphics* m_Graphics;
		std::unordered_map<Boxfish::SquareIndex, Marker> m_Markers;

	public:
		BoardMarkers(Layer* layer, const BoardGraphics* graphics);

		void AddMarker(const Boxfish::Square& square, const Color& color, MarkerType type = MarkerType::Square);
		void RemoveMarker(const Boxfish::Square& square);
		void Clear();
		void Invalidate();
	};

}
