#include <chess.hpp>
#include <string>
#include <iostream>
#include <limits>
#include <utility>

#define DEPTH 4

using namespace chess;

float negamax(Board& board, int depth, float alpha, float beta);

Move think(Board& board);

float heuristic(Board& board);

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

    //for (const auto &move : moves) {
    //    std::cout << uci::moveToUci(move) << std::endl;
    //}

    return 0;
}

Move think(Board& board)
{
    Movelist moves;
    movegen::legalmoves(moves, board);

    Move bestMove;
    float bestEvaluation = -std::numeric_limits<float>::infinity();

    for (const auto& move : moves)
    {
        board.makeMove(move);
        auto evaluateMove = -negamax(board, DEPTH, -std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity());
        board.unmakeMove(move);

        if (evaluateMove > bestEvaluation)
        {
            bestMove = move;
            bestEvaluation = evaluateMove;
        }
    }

    return bestMove;
}

float negamax(Board& board, int depth, float alpha, float beta)
{
    if (depth == 0)
        return heuristic(board);

    auto game_result = board.isGameOver();

    if (game_result.second != GameResult::NONE)
        return heuristic(board);

    float value = -std::numeric_limits<float>::infinity();

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
    Square max_square(64);
    for (Square i = 0; i < max_square; i++) {
        auto piece = board.at(i);
        if (piece && piece.color() == color) {
            score += piece.type() == PieceType::PAWN ? 1.0 :
                     piece.type() == PieceType::KNIGHT ? 3.0 :
                     piece.type() == PieceType::BISHOP ? 3.0 :
                     piece.type() == PieceType::ROOK ? 5.0 :
                     piece.type() == PieceType::QUEEN ? 9.0 : 0.0;
        }
    }
    return score;
}

float heuristic(Board& board)
{
    auto result = board.isGameOver();

    if (result.second == GameResult::DRAW)
        return 0.0;
    else if (result.second == GameResult::WIN)
        return std::numeric_limits<float>::infinity();
    else if (result.second == GameResult::LOSE)
        return -std::numeric_limits<float>::infinity();

    auto whitescore = calculateMaterial(board, Color::WHITE);
    auto blackscore = calculateMaterial(board, Color::BLACK);
    auto diff = whitescore - blackscore;

    return diff * (board.sideToMove() == Color::WHITE ? 1 : -1);
}