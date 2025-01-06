#include "search.hxx"
#include "tt.hxx"
#include "evaluate.hxx"
#include "uci.hxx"
#include <cstring>

using namespace chess;
using namespace Engine::TranspositionTable;
using namespace Engine::Evaluate;

extern uint32_t milliseconds_to_think;

namespace Engine::Search
{   
    Table table(1 << 28);
    Move killerMoves[256][2];

    void updatePV(Move PVup[256], const Move PVdown[256], const chess::Move& move)
    {
        PVup[0] = move;
        std::memcpy(&PVup[1], PVdown, sizeof(Move) * 255);
    }

    void updatePV(Move PV[256], const chess::Move& move)
    {
        PV[0] = move;
        PV[1] = 0;
    }

    Move iterativeDeepening(Board& board)
    {
        count = 0;
        maxPly = 0;

        begin = std::chrono::steady_clock::now();
        const auto boardCopy = board;

        Movelist moves;
        movegen::legalmoves(moves, board);

        Move bestMove = moves[0];

        try
        {
            // Iterative deepening
            for (auto depth = 1; depth < 256; depth++)
            {
                Move PV[256];
                auto score = search<NodeType::PV>(board, WORST_EVAL, BEST_EVAL, depth, 0, 0, PV);
                bestMove = PV[0];

                UCI::announceInfo(PV, depth, score, count);
            }
        }
        catch (const TimeOut& e)
        {
            board = boardCopy;
        }

        return bestMove;
    }

    template <NodeType nodeType>
    int32_t search(Board& board, int32_t alpha, int32_t beta, uint8_t depth, const uint8_t ply, uint8_t addedDepth, Move PV[256])
    {
        maxPly = std::max(ply, maxPly);

        // Search extension
        if (board.inCheck() && addedDepth < 3)
        {
            depth++;
            addedDepth++;
        }

        // TT
        auto ttentry = table.get_entry(board);
        auto alphaOrig = alpha;
        Move previousBestMove = 0;
        if (ttentry.hash == board.hash())
        {
            if (ttentry.depth >= depth)
            {
                if (ttentry.flag == EXACT)
                {
                    updatePV(PV, ttentry.bestMove);
                    return ttentry.value;
                }
                else if (ttentry.flag == LOWERBOUND)
                    alpha = std::max(alpha, ttentry.value);
                else if (ttentry.flag == UPPERBOUND)
                    beta = std::min(beta, ttentry.value);

                if (alpha >= beta)
                {
                    updatePV(PV, ttentry.bestMove);
                    return ttentry.value;
                }
            }

            previousBestMove = ttentry.bestMove;
        }

        // NMP
        /*if (depth >= 3 &&
            !board.inCheck() &&
            nodeType == NodeType::NonPV)
        {
            board.makeNullMove();
            auto sequence = search<NodeType::NonPV>(board, -beta, -beta + 1, depth - 3, ply + 1, settings, PVs, addedDepth);
            auto value = -sequence.get_evaluation();
            board.unmakeNullMove();

            if (value >= beta)
                return value;
        }*/

        // QS
        if (depth == 0)
            return quiescence(board, alpha, beta, ply + 1);

        Movelist moves;
        movegen::legalmoves(moves, board);
        
        if (auto gameover = isGameOver(board, moves))
            return heuristic(board, ply, gameover);

        // MVV-LVA
        orderMoves(moves, board, killerMoves[depth], previousBestMove);

        int32_t bestEval = WORST_EVAL;
        Move bestMove = 0;
        int movesSearched = 0;

        for (const auto& move : moves)
        {
            board.makeMove(move);
            int32_t value;
            Move PVdown[256];

            /*// LMR
            if (movesSearched >= 4 &&
                depth >= 3 &&
                move.score() == -9000) // score assigned for non-captures, non-killer moves, non best moves
            {
                sequence = search(board, -alpha - 1, -alpha, depth - 2, ply + 1, settings, PVs, isPV, true, addedDepth);
                value = -sequence.get_evaluation();
            }
            else
            {
                value = alpha + 1;
            }*/

            // PVS
            if (movesSearched == 0)
            {
                value = -search<nodeType>(board, -beta, -alpha, depth - 1, ply + 1, addedDepth, PVdown);
            }
            else
            {
                value = -search<NodeType::NonPV>(board, -alpha - 1, -alpha, depth - 1, ply + 1, addedDepth, PVdown);

                if (value > alpha && nodeType == NodeType::PV)
                {
                    value = -search<NodeType::PV>(board, -beta, -alpha, depth - 1, ply + 1, addedDepth, PVdown);
                }
            }

            /*
            if (movesSearched == 0)
            {
                sequence = search<nodeType>(board, -beta, -alpha, depth - 1, ply + 1, settings, PVs, addedDepth);
                value = -sequence.get_evaluation();
            }
            else
            {
                sequence = search<NodeType::NonPV>(board, -alpha - 1, -alpha, depth - 1, ply + 1, settings, PVs, addedDepth);
                value = -sequence.get_evaluation();

                if (value > alpha && nodeType == NodeType::PV)
                {
                    sequence = search<NodeType::PV>(board, -beta, -alpha, depth - 1, ply + 1, settings, PVs, addedDepth);
                    value = -sequence.get_evaluation();
                }
            }*/

            /*// Main search
            if (value > alpha)
            {
                sequence = search(board, -beta, -alpha, depth - 1, ply + 1, settings, PVs, isPV, true, addedDepth);
                value = -sequence.get_evaluation();
            }*/

            if (value > bestEval)
            {
                bestEval = value;
                bestMove = move;
                updatePV(PV, PVdown, move);
            }

            board.unmakeMove(move);

            // Alpha-beta pruning
            if (value > alpha)
            {
                alpha = value;
            }

            if (alpha >= beta)
            {
                // Killer moves
                if (!board.isCapture(move))
                {
                    killerMoves[depth][1] = killerMoves[depth][0];
                    killerMoves[depth][0] = move;
                }
                break;
            }

            movesSearched++;
        }

        // TT
        if (ttentry.hash != board.hash() || depth > ttentry.depth)
        {
            ttentry.value = bestEval;

            if (ttentry.value <= alphaOrig)
                ttentry.flag = UPPERBOUND;
            else if (ttentry.value >= beta)
                ttentry.flag = LOWERBOUND;
            else
                ttentry.flag = EXACT;

            ttentry.depth = depth;
            ttentry.hash = board.hash();
            ttentry.bestMove = bestMove;
            table.set_entry(board, ttentry);
        }

        return bestEval;
    }

    int32_t quiescence(Board& board, int32_t alpha, int32_t beta, const uint8_t ply)
    {
        maxPly = std::max(ply, maxPly);

        Movelist moves;
        movegen::legalmoves(moves, board);

        int value = heuristic(board, ply + 1, isGameOver(board, moves));

        if (value >= beta)
            return beta;
        else if (value > alpha)
            alpha = value;

        orderMoves(moves, board);

        for (const auto& move : moves)
        {
            if (!board.isCapture(move) && !board.inCheck())
                break;

            board.makeMove(move);
            value = -quiescence(board, -beta, -alpha, ply + 1);
            board.unmakeMove(move);

            if (value >= beta)
                return beta;
            else if (value > alpha)
                alpha = value;
        }

        return alpha;
    }
}