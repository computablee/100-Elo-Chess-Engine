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

    void orderMoves(Movelist& moves, const Board& board)
    {
        std::sort(moves.begin(), moves.end(), [&board](const Move& a, const Move& b) {
            if (board.isCapture(a) && !board.isCapture(b))
                return true;
            else if (!board.isCapture(a) && board.isCapture(b))
                return false;
            else if (board.isCapture(a) && board.isCapture(b))
            {
                if (board.at(a.to()) > board.at(b.to()))
                    return true;
                else if (board.at(a.to()) < board.at(b.to()))
                    return false;
                else
                    return board.at(a.from()) < board.at(b.from());
            }
            else return a.from() < b.from();
        });
    }

    void orderMoves(Movelist& moves, const Board& board, const Move& bestMove)
    {
        std::sort(moves.begin(), moves.end(), [&board, &bestMove](const Move& a, const Move& b) {
            if (a == bestMove)
                return true;
            else if (b == bestMove)
                return false;
            else if (board.isCapture(a) && !board.isCapture(b))
                return true;
            else if (!board.isCapture(a) && board.isCapture(b))
                return false;
            else if (board.isCapture(a) && board.isCapture(b))
            {
                if (board.at(a.to()) > board.at(b.to()))
                    return true;
                else if (board.at(a.to()) < board.at(b.to()))
                    return false;
                else
                    return board.at(a.from()) < board.at(b.from());
            }
            else return a.from() < b.from();
        });
    }
}