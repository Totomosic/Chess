#pragma once
#include "Bolt.h"
#include "BoardEvents.h"

namespace Chess
{

	class Board
	{
	public:
		struct MoveInfo
		{
		public:
			Boxfish::Move Move;
			Boxfish::UndoInfo Undo;
		};

	private:
		Boxfish::Position m_Position;
		std::vector<MoveInfo> m_MoveHistory;
		int m_MovePointer;
		int m_PieceIds[Boxfish::FILE_MAX * Boxfish::RANK_MAX];
		int m_NextPieceId;

		Boxfish::MovePool m_MovePool;

		std::unique_ptr<EventBus> m_EventBus;

	public:
		EventEmitter<PieceAdded> OnPieceAdded;
		EventEmitter<PieceRemoved> OnPieceRemoved;
		EventEmitter<PieceMoved> OnPieceMoved;
		EventEmitter<PiecePromoted> OnPiecePromoted;

	public:
		Board();

		const Boxfish::Position& GetPosition() const;
		Boxfish::Team GetTeamToMove() const;
		const std::vector<MoveInfo>& GetMoveHistory() const;
		int GetPieceIdAt(const Boxfish::Square& square) const;

		void SetPosition(const Boxfish::Position& position);
		void SetPositionFromFen(const std::string& fen);
		void SetStartingPosition();

		bool Move(const Boxfish::Move& move, bool animateMove = true);
		bool Move(const Boxfish::Square& from, const Boxfish::Square& to, bool animateMove = true);
		
		Boxfish::Move PopMove(bool animateMove = true);

	private:
		void Reset();
		void UndoMove(const MoveInfo& move, bool animateMove);
		void ClearPieces(bool sendEvents);
		int GetNextPieceId();
		void SendInitialEvents();

		void MovePiece(const Boxfish::Square& from, const Boxfish::Square& to, bool isPlayed, bool animateMove);
		void AddPiece(const Boxfish::Square& square, Boxfish::Piece piece, Boxfish::Team team);
		void RemovePiece(const Boxfish::Square& square);
		void PromotePiece(const Boxfish::Square& square, Boxfish::Piece promotion);
	};

}
