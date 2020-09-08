#include "Board.h"

namespace Chess
{

    Board::Board()
        : m_StartingPosition(), m_Position(), m_MoveHistory(), m_MovePointer(0), m_PieceIds(), m_NextPieceId(0), m_MovePool(Boxfish::MAX_MOVES * 1), m_EventBus(std::make_unique<EventBus>()), m_Playable(true),
        OnPieceAdded(m_EventBus->GetEmitter<PieceAdded>()),
        OnPieceRemoved(m_EventBus->GetEmitter<PieceRemoved>()),
        OnPieceMoved(m_EventBus->GetEmitter<PieceMoved>()),
        OnPiecePromoted(m_EventBus->GetEmitter<PiecePromoted>()),
        OnNewBoard(m_EventBus->GetEmitter<NewBoard>())
    {
        m_EventBus->SetImmediateMode(true);
        ClearPieces(false);
        Reset();
    }

    const Boxfish::Position& Board::GetPosition() const
    {
        return m_Position;
    }

    Boxfish::Team Board::GetTeamToMove() const
    {
        return GetPosition().TeamToPlay;
    }

    const std::vector<Board::MoveInfo>& Board::GetMoveHistory() const
    {
        return m_MoveHistory;
    }

    int Board::GetPieceIdAt(const Boxfish::Square& square) const
    {
        return m_PieceIds[Boxfish::BitBoard::SquareToBitIndex(square)];
    }

    std::vector<Boxfish::Move> Board::GetLegalMovesFor(const Boxfish::Square& square) const
    {
        Boxfish::MoveList moves = m_MovePool.GetList();
        Boxfish::MoveGenerator generator(m_Position);
        generator.GetPseudoLegalMoves(moves);
        generator.FilterLegalMoves(moves);

        std::vector<Boxfish::Move> result;
        for (int i = 0; i < moves.MoveCount; i++)
        {
            if (moves.Moves[i].GetFromSquare() == square)
                result.push_back(moves.Moves[i]);
        }
        return result;
    }

    bool Board::IsPlayable() const
    {
        return m_Playable;
    }

    void Board::SetPlayable(bool isPlayable)
    {
        m_Playable = isPlayable;
    }

    void Board::SetPosition(const Boxfish::Position& position)
    {
        Reset();
        m_Position = position;
        m_StartingPosition = position;
        SendInitialEvents();
        OnNewBoard.Emit({ m_Position });
    }

    void Board::SetPositionFromFen(const std::string& fen)
    {
        SetPosition(Boxfish::CreatePositionFromFEN(fen));
    }

    void Board::SetStartingPosition()
    {
        SetPosition(Boxfish::CreateStartingPosition());
    }

    bool Board::Move(const Boxfish::Move& move, bool animateMove)
    {
        Boxfish::MoveList legalMoves = m_MovePool.GetList();
        Boxfish::MoveGenerator generator(m_Position);
        generator.GetPseudoLegalMoves(legalMoves);
        generator.FilterLegalMoves(legalMoves);

        bool isLegal = false;
        for (int i = 0; i < legalMoves.MoveCount; i++)
        {
            if (legalMoves.Moves[i] == move)
            {
                isLegal = true;
                break;
            }
        }
        if (!isLegal)
        {
            BOX_INFO("Move {} is not legal", Boxfish::UCI::FormatMove(move));
            return false;
        }

        Boxfish::Square fromSquare = move.GetFromSquare();
        Boxfish::Square toSquare = move.GetToSquare();
        Boxfish::MoveFlag flags = move.GetFlags();
        Boxfish::Team teamToMove = GetTeamToMove();

        MoveInfo moveInfo;
        moveInfo.Move = move;
        Boxfish::ApplyMove(m_Position, move, &moveInfo.Undo);
        m_MoveHistory.push_back(moveInfo);

        if ((flags & Boxfish::MOVE_PROMOTION) && (flags & Boxfish::MOVE_CAPTURE))
        {
            PromotePiece(fromSquare, move.GetPromotionPiece());
            RemovePiece(toSquare);
        }
        else if (flags & Boxfish::MOVE_CAPTURE)
        {
            RemovePiece(toSquare);
        }
        else if (flags & Boxfish::MOVE_PROMOTION)
        {
            PromotePiece(fromSquare, move.GetPromotionPiece());
        }
        else if (flags & Boxfish::MOVE_EN_PASSANT)
        {
            Boxfish::SquareIndex index = Boxfish::BitBoard::SquareToBitIndex(toSquare);
            index = (Boxfish::SquareIndex)(index - Boxfish::GetForwardShift(teamToMove));
            RemovePiece(Boxfish::BitBoard::BitIndexToSquare(index));
        }
        else if (flags & Boxfish::MOVE_KINGSIDE_CASTLE)
        {
            MovePiece({ Boxfish::FILE_H, fromSquare.Rank }, { Boxfish::FILE_F, fromSquare.Rank }, false, animateMove);
        }
        else if (flags & Boxfish::MOVE_QUEENSIDE_CASTLE)
        {
            MovePiece({ Boxfish::FILE_A, fromSquare.Rank }, { Boxfish::FILE_D, fromSquare.Rank }, false, animateMove);
        }

        MovePiece(fromSquare, toSquare, true, animateMove);

        GameResult result = GetGameResult();
        if (result == GameResult::Checkmate)
        {
            std::cout << "Checkmate" << std::endl;
        }

        return true;
    }

    bool Board::Move(const Boxfish::Square& from, const Boxfish::Square& to, bool animateMove)
    {
        Boxfish::Move move = Boxfish::CreateMove(m_Position, from, to);
        if (Boxfish::SanityCheckMove(m_Position, move))
        {
            return Move(move, animateMove);
        }
        return false;
    }

    Boxfish::Move Board::PopMove(bool animateMove)
    {
        if (m_MoveHistory.size() > 0)
        {
            MoveInfo move = m_MoveHistory.back();
            m_MoveHistory.pop_back();
            UndoMove(move, animateMove);
            return move.Move;
        }
        return Boxfish::MOVE_NONE;
    }

    std::string Board::GetUCIString() const
    {
        std::string result = "position startpos";//fen " + Boxfish::GetFENFromPosition(m_StartingPosition);
        if (m_MoveHistory.size() > 0)
        {
            result += " moves";
            for (const MoveInfo& move : m_MoveHistory)
            {
                result += " " + Boxfish::UCI::FormatMove(move.Move);
            }
        }
        return result;
    }

    void Board::Reset()
    {
        ClearPieces(true);
        m_NextPieceId = 0;
        m_MoveHistory.clear();
        m_Position = Boxfish::Position();
    }

    void Board::UndoMove(const MoveInfo& moveInfo, bool animateMove)
    {
        Boxfish::Move move = moveInfo.Move;

        Boxfish::Square fromSquare = move.GetFromSquare();
        Boxfish::Square toSquare = move.GetToSquare();
        Boxfish::MoveFlag flags = move.GetFlags();
        Boxfish::Team teamToMove = GetTeamToMove();

        Boxfish::UndoMove(m_Position, move, moveInfo.Undo);

        MovePiece(toSquare, fromSquare, true, animateMove);

        if ((flags & Boxfish::MOVE_PROMOTION) && (flags & Boxfish::MOVE_CAPTURE))
        {
            AddPiece(toSquare, move.GetCapturedPiece(), teamToMove);
            PromotePiece(fromSquare, move.GetMovingPiece());
        }
        else if (flags & Boxfish::MOVE_CAPTURE)
        {
            AddPiece(toSquare, move.GetCapturedPiece(), teamToMove);
        }
        else if (flags & Boxfish::MOVE_PROMOTION)
        {
            PromotePiece(fromSquare, move.GetMovingPiece());
        }
        else if (flags & Boxfish::MOVE_EN_PASSANT)
        {
            Boxfish::SquareIndex index = Boxfish::BitBoard::SquareToBitIndex(toSquare);
            index = (Boxfish::SquareIndex)(index + Boxfish::GetForwardShift(teamToMove));
            AddPiece(Boxfish::BitBoard::BitIndexToSquare(index), Boxfish::PIECE_PAWN, GetTeamToMove());
        }
        else if (flags & Boxfish::MOVE_KINGSIDE_CASTLE)
        {
            MovePiece({ Boxfish::FILE_F, fromSquare.Rank }, { Boxfish::FILE_H, fromSquare.Rank }, false, animateMove);
        }
        else if (flags & Boxfish::MOVE_QUEENSIDE_CASTLE)
        {
            MovePiece({ Boxfish::FILE_D, fromSquare.Rank }, { Boxfish::FILE_A, fromSquare.Rank }, false, animateMove);
        }
    }

    void Board::ClearPieces(bool sendEvents)
    {
        for (Boxfish::SquareIndex sq = Boxfish::a1; sq < Boxfish::RANK_MAX * Boxfish::FILE_MAX; sq++)
        {
            int& pieceId = m_PieceIds[sq];
            if (pieceId >= 0 && sendEvents)
            {
                RemovePiece(Boxfish::BitBoard::BitIndexToSquare(sq));
            }
            pieceId = -1;
        }
    }

    int Board::GetNextPieceId()
    {
        return m_NextPieceId++;
    }

    void Board::SendInitialEvents()
    {
        Boxfish::Position position = m_Position;
        for (Boxfish::Piece piece = Boxfish::PIECE_PAWN; piece < Boxfish::PIECE_MAX; piece++)
        {
            Boxfish::BitBoard whitePieces = position.GetTeamPieces(Boxfish::TEAM_WHITE, piece);
            while (whitePieces)
            {
                Boxfish::SquareIndex squareIndex = Boxfish::PopLeastSignificantBit(whitePieces);
                Boxfish::Square square = Boxfish::BitBoard::BitIndexToSquare(squareIndex);
                AddPiece(square, piece, Boxfish::TEAM_WHITE);
            }
            Boxfish::BitBoard blackPieces = position.GetTeamPieces(Boxfish::TEAM_BLACK, piece);
            while (blackPieces)
            {
                Boxfish::SquareIndex squareIndex = Boxfish::PopLeastSignificantBit(blackPieces);
                Boxfish::Square square = Boxfish::BitBoard::BitIndexToSquare(squareIndex);
                AddPiece(square, piece, Boxfish::TEAM_BLACK);
            }
        }
    }

    GameResult Board::GetGameResult() const
    {
        Boxfish::EvaluationResult evaluation = Boxfish::EvaluateDetailed(m_Position);
        return GameResult::None;
    }

    void Board::MovePiece(const Boxfish::Square& from, const Boxfish::Square& to, bool isPlayed, bool animateMove)
    {
        Boxfish::SquareIndex fromIndex = Boxfish::BitBoard::SquareToBitIndex(from);
        Boxfish::SquareIndex toIndex = Boxfish::BitBoard::SquareToBitIndex(to);

        int pieceId = m_PieceIds[fromIndex];
        m_PieceIds[toIndex] = pieceId;
        m_PieceIds[fromIndex] = -1;

        PieceMoved movedEvent;
        movedEvent.PieceId = pieceId;
        movedEvent.From = from;
        movedEvent.To = to;
        movedEvent.Animate = animateMove;
        movedEvent.IsPlayedMove = isPlayed;
        OnPieceMoved.Emit(movedEvent);
    }

    void Board::AddPiece(const Boxfish::Square& square, Boxfish::Piece piece, Boxfish::Team team)
    {
        PieceAdded addedEvent;
        addedEvent.PieceId = GetNextPieceId();
        addedEvent.Square = square;
        addedEvent.Team = team;
        addedEvent.Type = piece;
        OnPieceAdded.Emit(addedEvent);
        m_PieceIds[Boxfish::BitBoard::SquareToBitIndex(square)] = addedEvent.PieceId;
    }

    void Board::RemovePiece(const Boxfish::Square& square)
    {
        int& pieceId = m_PieceIds[Boxfish::BitBoard::SquareToBitIndex(square)];
        PieceRemoved removedEvent;
        removedEvent.PieceId = pieceId;
        OnPieceRemoved.Emit(removedEvent);
        pieceId = -1;
    }

    void Board::PromotePiece(const Boxfish::Square& square, Boxfish::Piece promotion)
    {
        int pieceId = m_PieceIds[Boxfish::BitBoard::SquareToBitIndex(square)];
        PiecePromoted promotedEvent;
        promotedEvent.PieceId = pieceId;
        promotedEvent.Promotion = promotion;
        OnPiecePromoted.Emit(promotedEvent);
    }

}
