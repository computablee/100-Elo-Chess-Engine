#include "search.hxx"
#include "tt.hxx"
#include "evaluate.hxx"

using namespace chess;
using namespace Engine::TranspositionTable;
using namespace Engine::Evaluate;

namespace Engine::Search
{   
    Table table(1 << 28);

    Move iterativeDeepening(Board& board, const Settings& settings)
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
            for (auto depth = 1; ; depth++)
            {
                #ifdef DEBUG
                std::cout << "thinking for " << milliseconds_to_think << " milliseconds... depth = " << depth << std::endl;
                #endif

                auto bestMoveSequence = search(board, settings.get_worst_eval(), settings.get_best_eval(), depth, 1, board.sideToMove() == Color::WHITE ? 1 : -1, settings);
                bestMove = bestMoveSequence.get_first_move();
            }
        }
        catch (const TimeOut& e)
        {
            board = boardCopy;
        }

        return bestMove;
    }

    Sequence search(Board& board, int32_t alpha, int32_t beta, const int depth, const int ply, const int color, const Settings& settings)
    {
        maxPly = std::max(ply, maxPly);

#ifdef NMP
        if (depth >= 3 && !board.inCheck())
        {
            board.makeNullMove();
            int score = -negamax(board, depth - 3, -beta, -beta + 1, -color, maxDepth);
            board.unmakeNullMove();

            if (score >= beta)
                return score;
        }
#endif

        auto alphaOrig = alpha;

        auto ttentry = table.get_entry(board);

        Move previousBestMove;

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

        Movelist moves;
        movegen::legalmoves(moves, board);

        if (depth == 0)
            return Sequence(quiescence(board, alpha, beta, depth, ply + 1, color, settings));
        
        if (auto gameover = isGameOver(board, moves))
            return Sequence(heuristic(board, ply, gameover, settings) * color);

        const auto orderedMoves = orderMoves(moves, board, previousBestMove);

        Sequence bestSequence(settings.get_worst_eval());

        for (const auto& move : orderedMoves)
        {
            board.makeMove(move);
            auto sequence = search(board, -beta, -alpha, depth - 1, ply + 1, -color, settings);
            auto value = -sequence.get_evaluation();
            if (value > bestSequence.get_evaluation())
                bestSequence = Sequence(value, move, std::move(sequence));
            board.unmakeMove(move);

            if (value > alpha)
                alpha = value;

            if (alpha >= beta)
                break;
        }

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

    int32_t quiescence(Board& board, int32_t alpha, int32_t beta, const int depth, const int ply, const int color, const Settings& settings)
    {
        maxPly = std::max(ply, maxPly);

        Movelist moves;
        movegen::legalmoves(moves, board);

        int value = heuristic(board, ply + 1, isGameOver(board, moves), settings) * color;

        if (value >= beta)
            return beta;
        else if (value > alpha)
            alpha = value;

        const auto orderedMoves = orderMoves(moves, board);

        for (const auto& move : orderedMoves)
        {
            if (!board.isCapture(move))
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