#include <chess.hpp>
#include <string>
#include <iostream>
#include <limits>
#include <utility>
#include <bit>
#include <climits>
#include <chrono>
#include <list>
#include <unordered_map>
#include <thread>

#include "helpers.hxx"
#include "pst.hxx"
#include "settings.hxx"
#include "tt.hxx"
#include "uci.hxx"

using namespace chess;
using namespace Engine;
using namespace Engine::Helpers;
using namespace Engine::PST;
using namespace Engine::TranspositionTable;
using namespace Engine::UCI;

int negamax(Board& board, int depth, int alpha, int beta, int color, int maxDepth);
Move think(Board& board);
int heuristic(const Board& board, const int& distanceToMaxDepth, GameOverResult gameover);
int quiescence(Board& board, int alpha, int beta, const int depth, const int maxDepth, const int color);

static int count;
static int maxPly;
Settings settings;
PieceSquareTable pieceSquareTable;
Table table(1 << 28);

uint32_t milliseconds_to_think;

int main()
{
    Board board;

    parseStart(settings);

    count = 0;
    maxPly = 0;

    while (true)
    {
        milliseconds_to_think = parseEach(board);

        auto bestmove = think(board);

        #ifdef DEBUG
        std::cout << "evaluated " << count << " positions" << std::endl;
        std::cout << "maximum depth searched was " << maxPly << std::endl;
        #endif

        std::cout << "bestmove " << uci::moveToUci(bestmove) << std::endl;
        board.makeMove(bestmove);

        count = 0;
    }

    return 0;
}

std::list<Move> orderMoves(const Movelist& moves, const Board& board)
{
    std::list<Move> orderedMoves;
    for (const auto& move : moves)
        orderedMoves.push_back(move);

    orderedMoves.sort([&board](const Move& a, const Move& b) {
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
        else return true;
    });

    return orderedMoves;
}

std::list<Move> orderMoves(const Movelist& moves, const Board& board, const Move& bestMove)
{
    std::list<Move> orderedMoves;
    for (const auto& move : moves)
        orderedMoves.push_back(move);

    orderedMoves.sort([&board, &bestMove](const Move& a, const Move& b) {
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
        else return true;
    });

    return orderedMoves;
}

class TimeOut : std::exception { };

static std::chrono::steady_clock::time_point begin;

Move think(Board& board)
{
    begin = std::chrono::steady_clock::now();

    Board boardCopy = board;

    Movelist moves;
    movegen::legalmoves(moves, board);

    if (moves.size() == 1)
        return moves[0];

    Move bestMove = moves[0];
    int bestEvaluation = settings.get_worst_eval();

    try
    {
        for (auto depth = 1; ; depth++)
        {
            #ifdef DEBUG
            std::cout << "thinking for " << milliseconds_to_think << " milliseconds... depth = " << depth << std::endl;
            #endif

            const auto orderedMoves = orderMoves(moves, board, bestMove);

            for (const auto& move : orderedMoves)
            {
                board.makeMove(move);
                auto evaluateMove = -negamax(board, depth, settings.get_worst_eval(), settings.get_best_eval(), board.sideToMove() == Color::WHITE ? 1 : -1, depth);
                board.unmakeMove(move);

                if (evaluateMove > bestEvaluation)
                {
                    bestMove = move;
                    bestEvaluation = evaluateMove;
                }
            }
        }
    }
    catch (const TimeOut& e)
    {
        board = boardCopy;
    }

    return bestMove;
}

int negamax(Board& board, int depth, int alpha, int beta, int color, int maxDepth)
{
    maxPly = std::max(maxDepth - depth, maxPly);

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
                return ttentry.value;
            else if (ttentry.flag == LOWERBOUND)
                alpha = std::max(alpha, ttentry.value);
            else if (ttentry.flag == UPPERBOUND)
                beta = std::min(beta, ttentry.value);

            if (alpha >= beta)
                return ttentry.value;
        }

        previousBestMove = ttentry.bestMove;
    }

    int value = settings.get_worst_eval();

    Movelist moves;
    movegen::legalmoves(moves, board);

    if (depth == 0)
        return quiescence(board, alpha, beta, depth, maxDepth, color);
    
    if (auto gameover = isGameOver(board, moves))
        return heuristic(board, maxDepth - depth, gameover) * color;

    const auto orderedMoves = orderMoves(moves, board, previousBestMove);

    Move bestMove;

    for (const auto& move : orderedMoves)
    {
        board.makeMove(move);
        value = std::max(value, -negamax(board, depth - 1, -beta, -alpha, -color, maxDepth));
        board.unmakeMove(move);

        if (value > alpha)
        {
            alpha = value;
            bestMove = move;
        }

        if (alpha >= beta)
            break;
    }

    if (ttentry.hash != board.hash() || depth > ttentry.depth)
    {
        ttentry.value = value;

        if (value <= alphaOrig)
            ttentry.flag = UPPERBOUND;
        else if (value >= beta)
            ttentry.flag = LOWERBOUND;
        else
            ttentry.flag = EXACT;

        ttentry.depth = depth;
        ttentry.hash = board.hash();
        ttentry.bestMove = bestMove;
        table.set_entry(board, ttentry);
    }

    return value;
}

static const int pieceValues[] = { 100, 300, 300, 500, 900, 0 };
static const int gamePhase[] = { 0, 1, 1, 2, 4, 0 };

int calculateMaterial(const Board& board) {
    int score = 0;

    int mg[2] = { 0 };
    int eg[2] = { 0 };
    int phase = 0;

    Square max_square(64);
    for (Square i = 0; i < max_square; i++) {
        const auto piece = board.at(i);
        if (piece != Piece::NONE) {
            mg[static_cast<int>(piece.color())] += pieceValues[static_cast<int>(piece.type())]
                + pieceSquareTable.get_value_middlegame(piece.color(), piece.type(), i.index());
            eg[static_cast<int>(piece.color())] += pieceValues[static_cast<int>(piece.type())]
                + pieceSquareTable.get_value_endgame(piece.color(), piece.type(), i.index());
            phase += gamePhase[static_cast<int>(piece.type())];
        }
    }

    int mgScore = mg[0] - mg[1];
    int egScore = eg[0] - eg[1];
    if (phase > 24) phase = 24;
    int egPhase = 24 - phase;

    score = (mgScore * phase + egScore * egPhase) / 24;

    return score;
}

int heuristic(const Board& board, const int& distanceToMaxDepth, GameOverResult gameover)
{
    if ((++count & 1023) == 0)
    {
        if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - begin).count() > milliseconds_to_think)
            throw TimeOut();
    }
    
    if (gameover == DRAW)
        return 0;
    else if (gameover == WHITEWON)
       return settings.get_best_eval() - distanceToMaxDepth;
    else if (gameover == BLACKWON)
        return settings.get_worst_eval() + distanceToMaxDepth;

    return calculateMaterial(board);
}

int quiescence(Board& board, int alpha, int beta, const int depth, const int maxDepth, const int color)
{
    maxPly = std::max(maxDepth - depth, maxPly);

    Movelist moves;
    movegen::legalmoves(moves, board);

    int value = heuristic(board, maxDepth - depth, isGameOver(board, moves)) * color;

    if (value >= beta)
        return beta;
    else if (value > alpha)
        alpha = value;

    if (depth >= maxDepth + 10)
        return alpha;

    orderMoves(moves, board);

    for (const auto& move : moves)
    {
        if (!board.isCapture(move))
            continue;
        board.makeMove(move);
        value = -quiescence(board, -beta, -alpha, depth - 1, maxDepth, -color);
        board.unmakeMove(move);

        if (value >= beta)
            return beta;
        else if (value > alpha)
            alpha = value;
    }

    return alpha;
}