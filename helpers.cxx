#include "helpers.hxx"

using namespace chess;
using namespace Engine::Helpers;

GameOverResult Engine::Helpers::isGameOver(const Board& board, const Movelist& moves)
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