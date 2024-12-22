#include <chess.hpp>
#include <string>
#include <iostream>
#include <limits>
#include <utility>
#include <bit>

#define DEPTH 6
#define BEST_EVAL 1000000
#define WORST_EVAL (-1000000)

using namespace chess;

int negamax(Board& board, int depth, int alpha, int beta);
Move think(Board& board);
int heuristic(Board& board);

int main () {
    Board board = Board(chess::constants::STARTPOS);
    Movelist moves;
    std::string input;

    while (true)
    {
        std::cin >> input;
        Move move = uci::uciToMove(board, input);
        board.makeMove(move);

        auto bestmove = think(board);

        std::cout << "bestmove " << uci::moveToUci(bestmove) << std::endl;
        board.makeMove(bestmove);
    }

    return 0;
}

Move think(Board& board)
{
    Movelist moves;
    movegen::legalmoves(moves, board);

    Move bestMove;
    int bestEvaluation = WORST_EVAL;

    for (const auto& move : moves)
    {
        board.makeMove(move);
        auto evaluateMove = -negamax(board, DEPTH, WORST_EVAL, BEST_EVAL);
        board.unmakeMove(move);

        if (evaluateMove > bestEvaluation)
        {
            bestMove = move;
            bestEvaluation = evaluateMove;
        }
    }

    return bestMove;
}

int negamax(Board& board, int depth, int alpha, int beta)
{
    if (depth == 0)
        return heuristic(board);

    auto game_result = board.isGameOver();

    if (game_result.second != GameResult::NONE)
        return heuristic(board);

    int value = WORST_EVAL;

    Movelist moves;
    movegen::legalmoves(moves, board);

    for (const auto& move : moves)
    {
        board.makeMove(move);
        value = std::max(value, -negamax(board, depth - 1, -beta, -alpha));
        board.unmakeMove(move);

        alpha = std::max(alpha, value);
        if (alpha >= beta)
            break;
    }

    return value;
}

int calculateMaterial(const Board& board, Color color) {
    int score = 0;
    score += std::popcount(board.pieces(PieceType::PAWN, color).getBits()) * 100;
    score += std::popcount(board.pieces(PieceType::BISHOP, color).getBits()) * 300;
    score += std::popcount(board.pieces(PieceType::KNIGHT, color).getBits()) * 300;
    score += std::popcount(board.pieces(PieceType::ROOK, color).getBits()) * 500;
    score += std::popcount(board.pieces(PieceType::QUEEN, color).getBits()) * 900;

    return score;
}

int heuristic(Board& board)
{
    auto result = board.isGameOver();

    if (result.second == GameResult::DRAW)
        return 0;
    else if (result.second == GameResult::WIN)
        return BEST_EVAL;
    else if (result.second == GameResult::LOSE)
        return WORST_EVAL;

    auto whitescore = calculateMaterial(board, Color::WHITE);
    auto blackscore = calculateMaterial(board, Color::BLACK);
    auto diff = whitescore - blackscore;

    return diff * (board.sideToMove() == Color::WHITE ? 1 : -1);
}