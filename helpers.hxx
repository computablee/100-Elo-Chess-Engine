#pragma once
#include <cstdint>
#include <chess.hpp>

namespace Engine::Helpers
{
    enum GameOverResult : uint8_t { ONGOING, WHITEWON, BLACKWON, DRAW };

    GameOverResult isGameOver(const chess::Board& board, const chess::Movelist& moves);
}

constexpr inline int flip(const int x) { return (x ^ 56) & 0xFF; }

inline uint32_t reduce(const uint32_t x, const uint32_t N) { return ((uint64_t)x * (uint64_t)N) >> 32; }