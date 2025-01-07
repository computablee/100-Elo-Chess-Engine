#include "board.hxx"
#include "evaluate.hxx"
#include "pst.hxx"

using namespace chess;
using namespace Engine::PST;

namespace Engine
{
    static const int gamePhase[] = { 0, 1, 1, 2, 4, 0 };
    PieceSquareTable pieceSquareTable;

    Board::Board(const std::string& fen) : board(fen), phase(0)
    {
        this->fullEvaluation();
    }

    Board::Board() : board(chess::constants::STARTPOS), phase(0)
    {
        this->fullEvaluation();
    }

    void Board::makeMove(const chess::Move& move)
    {
        auto reEval = this->updateEvaluation(move);
        this->board.makeMove(move);
        if (reEval) this->fullEvaluation();
    }

    void Board::unmakeMove(const chess::Move& move)
    {
        this->board.unmakeMove(move);
        this->updateEvaluationBack(move);
    }

    void Board::makeNullMove()
    {
        this->board.makeNullMove();
    }

    void Board::unmakeNullMove()
    {
        this->board.unmakeNullMove();
    }

    void Board::fullEvaluation()
    {
        mg[0] = 0;
        mg[1] = 0;
        eg[0] = 0;
        eg[1] = 0;
        phase = 0;

        for (uint32_t i = 0; i < 64; i++) {
            const auto piece = board.at(i);
            if (piece != Piece::NONE) {
                mg[static_cast<int>(piece.color())] += pieceSquareTable.get_value_middlegame(piece.color(), piece.type(), i);
                eg[static_cast<int>(piece.color())] += pieceSquareTable.get_value_endgame(piece.color(), piece.type(), i);
                phase += gamePhase[static_cast<int>(piece.type())];
            }
        }
    }

    bool Board::updateEvaluation(const chess::Move& move)
    {
        if (move.typeOf() != chess::Move::NORMAL)
            return true;

        auto side = board.sideToMove();
        auto from = move.from().index();
        auto to = move.to().index();
        auto fromType = board.at(from).type();

        mg[static_cast<int>(side)] -= pieceSquareTable.get_value_middlegame(side, fromType, static_cast<uint8_t>(from));
        eg[static_cast<int>(side)] -= pieceSquareTable.get_value_endgame(side, fromType, static_cast<uint8_t>(from));
        mg[static_cast<int>(side)] += pieceSquareTable.get_value_middlegame(side, fromType, static_cast<uint8_t>(to));
        eg[static_cast<int>(side)] += pieceSquareTable.get_value_endgame(side, fromType, static_cast<uint8_t>(to));
        if (board.at(move.to()) != Piece::NONE)
        {
            auto toType = board.at(to).type();
            mg[static_cast<int>(~side)] -= pieceSquareTable.get_value_middlegame(~side, toType, static_cast<uint8_t>(to));
            eg[static_cast<int>(~side)] -= pieceSquareTable.get_value_endgame(~side, toType, static_cast<uint8_t>(to));
            phase -= gamePhase[static_cast<int>(toType)];
        }

        return false;
    }

    void Board::updateEvaluationBack(const chess::Move& move)
    {
        if (move.typeOf() != chess::Move::NORMAL)
        {
            fullEvaluation();
            return;
        }

        auto side = board.sideToMove();
        auto from = move.from().index();
        auto to = move.to().index();
        auto fromType = board.at(from).type();

        mg[static_cast<int>(side)] += pieceSquareTable.get_value_middlegame(side, fromType, static_cast<uint8_t>(from));
        eg[static_cast<int>(side)] += pieceSquareTable.get_value_endgame(side, fromType, static_cast<uint8_t>(from));
        mg[static_cast<int>(side)] -= pieceSquareTable.get_value_middlegame(side, fromType, static_cast<uint8_t>(to));
        eg[static_cast<int>(side)] -= pieceSquareTable.get_value_endgame(side, fromType, static_cast<uint8_t>(to));
        if (board.at(move.to()) != Piece::NONE)
        {
            auto toType = board.at(to).type();
            mg[static_cast<int>(~side)] += pieceSquareTable.get_value_middlegame(~side, toType, static_cast<uint8_t>(to));
            eg[static_cast<int>(~side)] += pieceSquareTable.get_value_endgame(~side, toType, static_cast<uint8_t>(to));
            phase += gamePhase[static_cast<int>(toType)];
        }
    }

    int32_t Board::getEvaluation() const
    {
        const int32_t mgScore = mg[0] - mg[1];
        const int32_t egScore = eg[0] - eg[1];
        const int mgPhase = phase > 24 ? 24 : phase;
        const int egPhase = 24 - mgPhase;

        int32_t score = (mgScore * mgPhase + egScore * egPhase) / 24;

        //assert(score == Engine::Evaluate::calculateMaterial(board));

        return score;
    }

    chess::Color Board::sideToMove() const
    {
        return board.sideToMove();
    }

    bool Board::isCapture(const chess::Move& move) const
    {
        return board.isCapture(move);
    }

    bool Board::inCheck() const
    {
        return board.inCheck();
    }

    Board::operator chess::Board&()
    {
        return board;
    }

    chess::Piece Board::at(const chess::Square& square) const
    {
        return board.at(square);
    }

    bool Board::isInsufficientMaterial() const
    {
        return board.isInsufficientMaterial();
    }

    bool Board::isRepetition() const
    {
        return board.isRepetition();
    }

    uint64_t Board::hash() const
    {
        return board.hash();
    }
}