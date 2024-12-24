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

#define BEST_EVAL 1000000
#define WORST_EVAL (-1000000)
#define TT_SIZE 10000000
#define ID_NAME "100 ELO Chess Engine (Quiescent)"
#define DEBUG
#define USE_TT
//#define NMP
#define EVAL 0

constexpr inline int flip(int x) { return (x ^ 56) & 0xFF; }

using namespace chess;

/* values from Rofchade, found on chessprogramming.org: https://www.chessprogramming.org/PeSTO%27s_Evaluation_Function */

static const int pawnTable[] = {
      0,   0,   0,   0,   0,   0,  0,   0,
     98, 134,  61,  95,  68, 126, 34, -11,
     -6,   7,  26,  31,  65,  56, 25, -20,
    -14,  13,   6,  21,  23,  12, 17, -23,
    -27,  -2,  -5,  12,  17,   6, 10, -25,
    -26,  -4,  -4, -10,   3,   3, 33, -12,
    -35,  -1, -20, -23, -15,  24, 38, -22,
      0,   0,   0,   0,   0,   0,  0,   0,
};

static const int knightTable[] = {
    -167, -89, -34, -49,  61, -97, -15, -107,
     -73, -41,  72,  36,  23,  62,   7,  -17,
     -47,  60,  37,  65,  84, 129,  73,   44,
      -9,  17,  19,  53,  37,  69,  18,   22,
     -13,   4,  16,  13,  28,  19,  21,   -8,
     -23,  -9,  12,  10,  19,  17,  25,  -16,
     -29, -53, -12,  -3,  -1,  18, -14,  -19,
    -105, -21, -58, -33, -17, -28, -19,  -23,
};

static const int bishopTable[] = {
    -29,   4, -82, -37, -25, -42,   7,  -8,
    -26,  16, -18, -13,  30,  59,  18, -47,
    -16,  37,  43,  40,  35,  50,  37,  -2,
     -4,   5,  19,  50,  37,  37,   7,  -2,
     -6,  13,  13,  26,  34,  12,  10,   4,
      0,  15,  15,  15,  14,  27,  18,  10,
      4,  15,  16,   0,   7,  21,  33,   1,
    -33,  -3, -14, -21, -13, -12, -39, -21,
};

static const int rookTable[] = {
     32,  42,  32,  51, 63,  9,  31,  43,
     27,  32,  58,  62, 80, 67,  26,  44,
     -5,  19,  26,  36, 17, 45,  61,  16,
    -24, -11,   7,  26, 24, 35,  -8, -20,
    -36, -26, -12,  -1,  9, -7,   6, -23,
    -45, -25, -16, -17,  3,  0,  -5, -33,
    -44, -16, -20,  -9, -1, 11,  -6, -71,
    -19, -13,   1,  17, 16,  7, -37, -26,
};

static const int queenTable[] = {
    -28,   0,  29,  12,  59,  44,  43,  45,
    -24, -39,  -5,   1, -16,  57,  28,  54,
    -13, -17,   7,   8,  29,  56,  47,  57,
    -27, -27, -16, -16,  -1,  17,  -2,   1,
     -9, -26,  -9, -10,  -2,  -4,   3,  -3,
    -14,   2, -11,  -2,  -5,   2,  14,   5,
    -35,  -8,  11,   2,   8,  15,  -3,   1,
     -1, -18,  -9,  10, -15, -25, -31, -50,
};

static const int kingTable[] = {
    -65,  23,  16, -15, -56, -34,   2,  13,
     29,  -1, -20,  -7,  -8,  -4, -38, -29,
     -9,  24,   2, -16, -20,   6,  22, -22,
    -17, -20, -12, -27, -30, -25, -14, -36,
    -49,  -1, -27, -39, -46, -44, -33, -51,
    -14, -14, -22, -46, -44, -30, -15, -27,
      1,   7,  -8, -64, -43, -16,   9,   8,
    -15,  36,  12, -54,   8, -28,  24,  14,
};

static int tables[2][6][64];
static int milliseconds_to_think;

int negamax(Board& board, int depth, int alpha, int beta, int color, int maxDepth);
Move think(Board& board);
int heuristic(const Board& board, const int& distanceToMaxDepth, const Movelist& moves);
int quiescence(Board& board, int alpha, int beta, const int depth, const int maxDepth, const int color);

#ifdef USE_TT
enum ttFlag : uint8_t { EXACT, UPPERBOUND, LOWERBOUND };

struct ttEntry
{
    uint64_t hash;
    int value;
    ttFlag flag;
    uint8_t depth;
};

ttEntry* transpositionTable;
#endif

static int count;

inline uint32_t reduce(const uint32_t x, const uint32_t N) {
  return ((uint64_t)x * (uint64_t)N) >> 32;
}

void parseGo(const Board& board, std::string input)
{
    int wtime, btime, winc, binc;
    std::stringstream stream;
    stream.str(input);
    std::string part;
    stream >> part;
    assert(part == "go");
    stream >> part;
    assert(part == "wtime");
    stream >> wtime;
    stream >> part;
    assert(part == "btime");
    stream >> btime;
    stream >> part;
    assert(part == "winc");
    stream >> winc;
    stream >> part;
    assert(part == "binc");
    stream >> binc;

    if (board.sideToMove() == Color::WHITE)
        milliseconds_to_think = wtime / 20 + winc / 2;
    else
        milliseconds_to_think = btime / 20 + binc / 2;
}

void parsePosition(Board& board, const std::string& line)
{
    std::stringstream stream;
    stream.str(line);
    std::string part;
    std::string start;
    stream >> part;
    assert(part == "position");
    stream >> part;
    if (part == "startpos")
        start = chess::constants::STARTPOS;
    else if (part == "fen")
    {
        stream >> part;
        start = part;
    }
    else assert(false);

    board = Board(start);

    stream >> part;
    if (part == "moves")
    {
        stream >> part;
        while (!stream.eof())
        {
            board.makeMove(uci::uciToMove(board, part));
            stream >> part;
        }
        
        board.makeMove(uci::uciToMove(board, part));
    }
}

void parseStart(Board& board)
{
    std::string line;
    auto progress = false;

    while (!progress)
    {
        std::getline(std::cin, line);

        if (line == "uci")
        {
            std::cout << "id name " << ID_NAME << std::endl;
            std::cout << "id author Phillip Lane" << std::endl;
            std::cout << "uciok" << std::endl;
        }
        else if (line == "isready")
        {
            std::cout << "readyok" << std::endl;
            progress = true;
        }
    }
}

void parseEach(Board& board)
{
    std::string line;
    auto progress = false;

    while (!progress)
    {
        std::getline(std::cin, line);
        
        if (line.substr(0, 8) == "position")
        {
            parsePosition(board, line);
        }
        else if (line == "isready")
        {
            std::cout << "readyok" << std::endl;
        }
        else if (line.substr(0, 2) == "go")
        {
            parseGo(board, line);
            progress = true;
        }
        else if (line == "quit")
        {
            exit(0);
        }
    }
}

int main()
{
    Board board;
    parseStart(board);
    std::string input;
#ifdef USE_TT
    transpositionTable = new ttEntry[TT_SIZE];
#endif
    count = 0;

    for (auto i = 0; i < 64; i++)
    {
        tables[0][0][i] = pawnTable[flip(i)];
        tables[1][0][i] = pawnTable[i];
        tables[0][1][i] = knightTable[flip(i)];
        tables[1][1][i] = knightTable[i];
        tables[0][2][i] = bishopTable[flip(i)];
        tables[1][2][i] = bishopTable[i];
        tables[0][3][i] = rookTable[flip(i)];
        tables[1][3][i] = rookTable[i];
        tables[0][4][i] = queenTable[flip(i)];
        tables[1][4][i] = queenTable[i];
        tables[0][5][i] = kingTable[flip(i)];
        tables[1][5][i] = kingTable[i];
    }

    while (true)
    {
        parseEach(board);
        auto bestmove = think(board);

        #ifdef DEBUG
        std::cout << "evaluated " << count << " positions" << std::endl;
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
    int bestEvaluation = WORST_EVAL;

    try
    {
        for (auto depth = 1; ; depth++)
        {
            #ifdef DEBUG
            std::cout << "thinking for " << milliseconds_to_think << " milliseconds... depth = " << depth << std::endl;
            #endif

            const auto orderedMoves = orderMoves(moves, board);

            for (const auto& move : orderedMoves)
            {
                board.makeMove(move);
                auto evaluateMove = -negamax(board, depth, WORST_EVAL, BEST_EVAL, board.sideToMove() == Color::WHITE ? 1 : -1, depth);
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

bool isGameOver(const Board& board, const Movelist& moves, bool& draw, bool& whiteWon, bool& blackWon)
{
    if (board.isInsufficientMaterial() || board.isRepetition())
    {
        draw = true;
        return true;
    }

    if (moves.empty())
    {
        if (board.inCheck() && board.sideToMove() == Color::WHITE)
            blackWon = true;
        else if (board.inCheck() && board.sideToMove() == Color::BLACK)
            whiteWon = true;
        else
            draw = true;

        return true;
    }

    return false;
}

int negamax(Board& board, int depth, int alpha, int beta, int color, int maxDepth)
{

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

#ifdef USE_TT
    auto alphaOrig = alpha;

    auto ttentry = transpositionTable[reduce(board.hash(), TT_SIZE)];

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
    }
#endif

    int value = WORST_EVAL;

    Movelist moves;
    movegen::legalmoves(moves, board);

    bool _, __, ___;

    if (depth == 0)
        return quiescence(board, alpha, beta, depth, maxDepth, color);
    
    if(isGameOver(board, moves, _, __, ___))
        return heuristic(board, maxDepth - depth, moves) * color;

    const auto orderedMoves = orderMoves(moves, board);

    for (const auto& move : orderedMoves)
    {
        board.makeMove(move);
        value = std::max(value, -negamax(board, depth - 1, -beta, -alpha, -color, maxDepth));
        board.unmakeMove(move);

        alpha = std::max(alpha, value);
        if (alpha >= beta)
            break;
    }

#ifdef USE_TT
    ttentry.value = value;

    if (value <= alphaOrig)
        ttentry.flag = UPPERBOUND;
    else if (value >= beta)
        ttentry.flag = LOWERBOUND;
    else
        ttentry.flag = EXACT;

    ttentry.depth = depth;
    ttentry.hash = board.hash();
    transpositionTable[reduce(board.hash(), TT_SIZE)] = ttentry;
#endif

    return value;
}

static const int pieceValues[] = { 100, 300, 300, 500, 900, 0 };

int calculateMaterial(const Board& board, Color color) {
    int score = 0;

#if EVAL == 0
    Square max_square(64);
    for (Square i = 0; i < max_square; i++) {
        const auto piece = board.at(i);
        if (piece != Piece::NONE && piece.color() == color) {
            score += pieceValues[static_cast<int>(piece.type())]
                + tables[static_cast<int>(color)][static_cast<int>(piece.type())][i.index()];
        }
    }
#else
    score += std::popcount(board.pieces(PieceType::PAWN, color).getBits());
    score += std::popcount(board.pieces(PieceType::BISHOP, color).getBits()) * 3;
    score += std::popcount(board.pieces(PieceType::KNIGHT, color).getBits()) * 3;
    score += std::popcount(board.pieces(PieceType::ROOK, color).getBits()) * 5;
    score += std::popcount(board.pieces(PieceType::QUEEN, color).getBits()) * 9;
#endif

    return score;
}

int heuristic(const Board& board, const int& distanceToMaxDepth, const Movelist& moves)
{
    if ((++count & 1023) == 0)
    {
        if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - begin).count() > milliseconds_to_think)
            throw TimeOut();
    }

    auto draw = false, whiteWon = false, blackWon = false;

    if (isGameOver(board, moves, draw, whiteWon, blackWon))
    {
        if (draw)
            return 0;
        else if (whiteWon)
           return BEST_EVAL - distanceToMaxDepth;
        else if (blackWon)
            return WORST_EVAL + distanceToMaxDepth;
        return 0;
    }

    auto whitescore = calculateMaterial(board, Color::WHITE);
    auto blackscore = calculateMaterial(board, Color::BLACK);
    return whitescore - blackscore;
}

int quiescence(Board& board, int alpha, int beta, const int depth, const int maxDepth, const int color)
{
    Movelist moves;
    movegen::legalmoves(moves, board);

    int value = heuristic(board, maxDepth - depth, moves) * color;

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