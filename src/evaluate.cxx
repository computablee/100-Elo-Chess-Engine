#include <cstdint>
#include "evaluate.hxx"
#include "pst.hxx"
#include "search.hxx"

using namespace chess;
using namespace Engine::PST;
using namespace Engine::Helpers;
using namespace Engine::Search;

extern uint32_t milliseconds_to_think;

namespace Engine::Evaluate
{
    PieceSquareTable pieceSquareTable;

    int32_t heuristic(const Board& board, const int ply, GameOverResult gameover)
    {
        int8_t color = board.sideToMove() == Color::WHITE ? 1 : -1;

        if ((++count & 1023) == 0)
        {
            if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - begin).count() > milliseconds_to_think)
                throw TimeOut();
        }

        if (gameover == DRAW)
            return 0;
        else if (gameover == WHITEWON)
           return (BEST_EVAL - ply) * color;
        else if (gameover == BLACKWON)
            return (WORST_EVAL + ply) * color;

        return calculateMaterial(board) * color;
    }

    static const int gamePhase[] = { 0, 1, 1, 2, 4, 0 };

    int32_t calculateMaterial(const Board& board)
    {
        int32_t score = 0;

        int32_t mg[2] = { 0 };
        int32_t eg[2] = { 0 };
        int phase = 0;

        for (uint32_t i = 0; i < 64; i++) {
            const auto piece = board.at(i);
            if (piece != Piece::NONE) {
                mg[static_cast<int>(piece.color())] += pieceSquareTable.get_value_middlegame(piece.color(), piece.type(), i);
                eg[static_cast<int>(piece.color())] += pieceSquareTable.get_value_endgame(piece.color(), piece.type(), i);
                phase += gamePhase[static_cast<int>(piece.type())];
            }
        }

        const int32_t mgScore = mg[0] - mg[1];
        const int32_t egScore = eg[0] - eg[1];
        if (phase > 24) phase = 24;
        int egPhase = 24 - phase;

        score = (mgScore * phase + egScore * egPhase) / 24;

        return score;
    }
}