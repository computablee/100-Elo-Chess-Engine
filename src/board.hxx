#ifndef BOARD
#define BOARD

#include <chess.hpp>
#include <cstdint>

namespace Engine
{
    class Board
    {
    private:
        chess::Board board;
        int32_t mg[2];
        int32_t eg[2];
        int phase;

        void fullEvaluation();

        bool updateEvaluation(const chess::Move& move);

        void updateEvaluationBack(const chess::Move& move);

    public:
        Board(const std::string& fen);

        Board();

        operator chess::Board&();

        void makeMove(const chess::Move& move);

        void unmakeMove(const chess::Move& move);

        void makeNullMove();

        void unmakeNullMove();

        int32_t getEvaluation() const;

        chess::Color sideToMove() const;

        bool isCapture(const chess::Move& move) const;

        bool inCheck() const;

        chess::Piece at(const chess::Square& square) const;

        bool isInsufficientMaterial() const;

        bool isRepetition() const;

        uint64_t hash() const;
    };
}

#endif