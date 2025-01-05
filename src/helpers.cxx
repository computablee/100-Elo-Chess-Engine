#include "helpers.hxx"

using namespace chess;
using namespace Engine::Helpers;

namespace Engine::Helpers
{
    GameOverResult isGameOver(const Board& board, const Movelist& moves)
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

    void orderMoves(Movelist& moves, const Board& board, const Move killerMove[2])
    {
        for (auto& move : moves)
        {
            if (move == killerMove[0])
                move.setScore(9000);
            else if (move == killerMove[1])
                move.setScore(8000);
            else if (board.isCapture(move))
                move.setScore((board.at(move.to()).type() * 1000 - board.at(move.from()).type() * 100));
            else
                move.setScore(-9000);
        }

        std::sort(moves.begin(), moves.end(), [](const Move& a, const Move& b) {
            const auto score_a = a.score();
            const auto score_b = b.score();

            if (score_a == score_b)
                return a.from() > b.from();
            else
                return score_a > score_b;
        });
    }

    void orderMoves(Movelist& moves, const Board& board, const Move killerMove[2], const Move& bestMove)
    {
        for (auto& move : moves)
        {
            if (move == bestMove)
                move.setScore(10000);
            else if (move == killerMove[0])
                move.setScore(9000);
            else if (move == killerMove[1])
                move.setScore(8000);
            else if (board.isCapture(move))
                move.setScore((board.at(move.to()).type() * 1000 - board.at(move.from()).type() * 100));
            else
                move.setScore(-9000);
        }

        std::sort(moves.begin(), moves.end(), [](const Move& a, const Move& b) {
            const auto score_a = a.score();
            const auto score_b = b.score();

            if (score_a == score_b)
                return a.from() > b.from();
            else
                return score_a > score_b;
        });
    }
}