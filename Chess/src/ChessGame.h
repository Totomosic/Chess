#pragma once
#include "Bolt.h"
#include "Board.h"
#include "BoardGraphics.h"
#include "BoardMarkers.h"
#include "Players/BoxfishPlayer.h"
#include "PlayerManager.h"
#include "BoardAnalyzer.h"

namespace Chess
{

	struct SceneData
	{
	public:
		Bolt::Scene* Scene;
		Layer* BoardLayer;
		Layer* PreviousMoveLayer;
		Layer* ValidMovesLayer;
		Layer* PiecesLayer;
		Layer* UILayer;
	};

	class ChessGame
	{
	private:
		Window* m_Window;
		SceneData m_SceneData;

		ScopedEntityHandle m_Camera;
		ScopedEntityHandle m_Background;

		Board m_Board;
		BoardGraphics m_BoardGraphics;
		BoardMarkers m_PreviousMoveMarkers;
		BoardMarkers m_ValidMoveMarkers;
		BoardAnalyzer m_Analyzer;

		PlayerManager m_Players;

		ScopedEventListener m_WindowResizeListener;
		ScopedEventListener m_MouseMoveListener;
		std::function<void()> m_OnExit;

		int m_SelectedPieceId;
		Boxfish::Square m_SelectedPieceSquare;

	public:
		ChessGame(Window* window, const SceneData& sceneData);
		ChessGame(const ChessGame& other) = delete;
		ChessGame& operator=(const ChessGame& other) = delete;
		ChessGame(ChessGame&& other) = delete;
		ChessGame& operator=(ChessGame&& other) = delete;
		~ChessGame() = default;

		template<typename T, typename ... Args>
		void AddPlayer(Args&& ... args)
		{
			m_Players.AddPlayer<T>(std::forward<Args>(args)...);
		}

		void Start();
		void Update();

		void SetExitCallback(const std::function<void()>& callback);

	private:
		bool HasSelectedPiece() const;
		void DeselectPiece();
		void DrawValidMoves(const std::vector<Boxfish::Move>& moves);
		void DrawPieceAtPosition(int pieceId, const Vector3f& position);
		void DrawPieceAtSquare(int pieceId);

		void UpdateAllGraphics();
		void SetupEventListeners();

	};

}