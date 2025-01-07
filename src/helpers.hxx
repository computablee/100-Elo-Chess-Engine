#pragma once
#include <cstdint>
#include <chess.hpp>
#include <list>
#include "board.hxx"

namespace Engine::Helpers
{
    enum GameOverResult : uint8_t { ONGOING, WHITEWON, BLACKWON, DRAW };

    GameOverResult isGameOver(const Engine::Board& board, const chess::Movelist& moves);

    void orderMoves(chess::Movelist& moves, const Engine::Board& board);

    void orderMoves(chess::Movelist& moves, const Engine::Board& board, const chess::Move killerMove[2]);

    void orderMoves(chess::Movelist& moves, const Engine::Board& board, const chess::Move killerMove[2], const chess::Move& bestMove);

    constexpr inline int32_t flip(const int32_t x) { return (x ^ 56) & 0xFF; }

    constexpr inline uint32_t reduce(const uint32_t x, const uint32_t N) { return ((uint64_t)x * (uint64_t)N) >> 32; }
}