#include "search.hxx"
#include "tt.hxx"
#include "evaluate.hxx"
#include "uci.hxx"

using namespace chess;
using namespace Engine::TranspositionTable;
using namespace Engine::Evaluate;

extern uint32_t milliseconds_to_think;

namespace Engine::Search
{   
    Table table(1 << 28);
    Move killerMoves[256][2];

    Move iterativeDeepening(Board& board, const Settings& settings)
    {
        count = 0;
        maxPly = 0;

        begin = std::chrono::steady_clock::now();
        const auto boardCopy = board;

        Movelist moves;
        movegen::legalmoves(moves, board);

        Move bestMove = moves[0];
        std::vector<Move> bestSequence;

        try
        {
            // Iterative deepening
            for (auto depth = 1; depth < 256; depth++)
            {
                auto bestMoveSequence = search(board, settings.get_worst_eval(), settings.get_best_eval(), depth, 0, board.sideToMove() == Color::WHITE ? 1 : -1,
                    settings, bestSequence, true, true, 0);
                auto score = bestMoveSequence.get_evaluation();
                auto sequence = std::move(bestMoveSequence).get_sequence();
                bestMove = sequence.front();
                bestSequence = std::vector<Move> { sequence.begin(), sequence.end() };

                UCI::announceInfo(bestSequence, depth, score, count);
            }
        }
        catch (const TimeOut& e)
        {
            board = boardCopy;
        }

        return bestMove;
    }

    Sequence search(Board& board, int32_t alpha, int32_t beta, uint8_t depth, const uint8_t ply, const int8_t color, const Settings& settings,
            const std::vector<chess::Move>& PVs, const bool PV, const bool canNMP, uint8_t addedDepth)
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
                    return Sequence(ttentry.value, std::list<Move> { ttentry.bestMove });
                else if (ttentry.flag == LOWERBOUND)
                    alpha = std::max(alpha, ttentry.value);
                else if (ttentry.flag == UPPERBOUND)
                    beta = std::min(beta, ttentry.value);

                if (alpha >= beta)
                    return Sequence(ttentry.value, std::list<Move> { ttentry.bestMove });
            }

            previousBestMove = ttentry.bestMove;
        }

        // NMP
        if (depth >= 3 &&
            !board.inCheck() &&
            !PV && canNMP)
        {
            board.makeNullMove();
            auto sequence = search(board, -beta, -beta + 1, depth - 3, ply + 1, -color, settings, PVs, false, false, addedDepth);
            auto value = -sequence.get_evaluation();
            board.unmakeNullMove();

            if (value >= beta)
                return value;
        }

        // QS
        if (depth == 0)
            return Sequence(quiescence(board, alpha, beta, depth, ply + 1, color, settings));

        Movelist moves;
        movegen::legalmoves(moves, board);
        
        if (auto gameover = isGameOver(board, moves))
            return Sequence(heuristic(board, ply, gameover, settings) * color);

        // MVV-LVA
        orderMoves(moves, board, killerMoves[depth], previousBestMove);

        Sequence bestSequence(settings.get_worst_eval());
        int movesSearched = 0;

        for (const auto& move : moves)
        {
            const auto isPV = PV && ply < PVs.size() && PVs[ply] == move;

            board.makeMove(move);
            Sequence sequence;
            int32_t value;

            // LMR
            if (movesSearched >= 4 &&
                depth >= 3 &&
                move.score() == -9000) // score assigned for non-captures, non-killer moves, non best moves
            {
                sequence = search(board, -alpha - 1, -alpha, depth - 2, ply + 1, -color, settings, PVs, isPV, true, addedDepth);
                value = -sequence.get_evaluation();
            }
            else
            {
                value = alpha + 1;
            }

            // Main search
            if (value > alpha)
            {
                sequence = search(board, -beta, -alpha, depth - 1, ply + 1, -color, settings, PVs, isPV, true, addedDepth);
                value = -sequence.get_evaluation();
            }

            if (value > bestSequence.get_evaluation())
                bestSequence = Sequence(value, move, std::move(sequence));

            board.unmakeMove(move);

            // Alpha-beta pruning
            if (value > alpha)
                alpha = value;

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
            ttentry.value = bestSequence.get_evaluation();

            if (ttentry.value <= alphaOrig)
                ttentry.flag = UPPERBOUND;
            else if (ttentry.value >= beta)
                ttentry.flag = LOWERBOUND;
            else
                ttentry.flag = EXACT;

            ttentry.depth = depth;
            ttentry.hash = board.hash();
            ttentry.bestMove = bestSequence.get_first_move();
            table.set_entry(board, ttentry);
        }

        return bestSequence;
    }

    int32_t quiescence(Board& board, int32_t alpha, int32_t beta, const uint8_t depth, const uint8_t ply, const int8_t color, const Settings& settings)
    {
        maxPly = std::max(ply, maxPly);

        Movelist moves;
        movegen::legalmoves(moves, board);

        int value = heuristic(board, ply + 1, isGameOver(board, moves), settings) * color;

        if (value >= beta)
            return beta;
        else if (value > alpha)
            alpha = value;

        orderMoves(moves, board, killerMoves[depth]);

        for (const auto& move : moves)
        {
            if (!board.isCapture(move) && !board.inCheck())
                break;

            board.makeMove(move);
            value = -quiescence(board, -beta, -alpha, depth - 1, ply + 1, -color, settings);
            board.unmakeMove(move);

            if (value >= beta)
                return beta;
            else if (value > alpha)
                alpha = value;
        }

        return alpha;
    }
}