#include "helpers.hxx"

using namespace chess;
using namespace Engine::Helpers;

namespace Engine::Helpers
{
    GameOverResult isGameOver(const Engine::Board& board, const Movelist& moves)
    {
        if (board.isInsufficientMaterial() || board.isRepetition())
        {
            return DRAW;
        }

        if (moves.empty())
        {
            if (board.inCheck() && board.sideToMove() == Color::WHITE)
                return BLACKWON;
            else if (board.inCheck() && board.sideToMove() == Color::BLACK)
                return WHITEWON;
            else
                return DRAW;
        }

        return ONGOING;
    }

    void orderMoves(Movelist& moves, const Engine::Board& board)
    {
        for (auto& move : moves)
        {
            if (board.isCapture(move))
                move.setScore((board.at(move.to()).type() * 1000 - board.at(move.from()).type() * 100));
            else
                move.setScore(-9000);
        }

        std::sort(moves.begin(), moves.end(), [](const Move& a, const Move& b) { return a.score() > b.score(); });
    }

    void orderMoves(Movelist& moves, const Engine::Board& board, const Move killerMove[2])
    {
        for (auto& move : moves)
        {
            if (board.isCapture(move))
                move.setScore((board.at(move.to()).type() * 1000 - board.at(move.from()).type() * 100));
            else if (move == killerMove[0])
                move.setScore(-7000);
            else if (move == killerMove[1])
                move.setScore(-8000);
            else
                move.setScore(-9000);
        }

        std::sort(moves.begin(), moves.end(), [](const Move& a, const Move& b) { return a.score() > b.score(); });
    }

    void orderMoves(Movelist& moves, const Engine::Board& board, const Move killerMove[2], const Move& bestMove)
    {
        for (auto& move : moves)
        {
            if (move == bestMove)
                move.setScore(10000);
            else if (board.isCapture(move))
                move.setScore((board.at(move.to()).type() * 1000 - board.at(move.from()).type() * 100));
            else if (move == killerMove[0])
                move.setScore(-7000);
            else if (move == killerMove[1])
                move.setScore(-8000);
            else
                move.setScore(-9000);
        }

        std::sort(moves.begin(), moves.end(), [](const Move& a, const Move& b) { return a.score() > b.score(); });
    }
}