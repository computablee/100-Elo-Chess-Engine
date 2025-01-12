#pragma once
#include <cstdint>
#include <chess.hpp>
#include <list>
#include <type_traits>
#include <utility>
#include <algorithm>
#include <climits>
#include <cmath>
#include "board.hxx"

namespace Engine::Helpers
{
    using MoveType = std::remove_reference<decltype(chess::Move().move())>::type;

    enum GameOverResult : uint8_t { ONGOING, WHITEWON, BLACKWON, DRAW };

    GameOverResult isGameOver(const Engine::Board& board, const chess::Movelist& moves);

    void orderMoves(chess::Movelist& moves, const Engine::Board& board);

    void orderMoves(chess::Movelist& moves, const Engine::Board& board, const chess::Move killerMove[2], const int16_t* historyMoves, const chess::Move& bestMove);

    constexpr inline int32_t flip(const int32_t x) { return (x ^ 56) & 0xFF; }

    constexpr inline __attribute__((always_inline)) uint32_t reduce(const uint64_t x, const uint64_t N) { return ((unsigned __int128)x * (unsigned __int128)N) >> 64; }

    constexpr inline void history_update(const MoveType move, int16_t* history, int16_t bonus)
    {
        int16_t clampedBonus = std::clamp(static_cast<int32_t>(bonus), INT16_MIN, INT16_MAX);
        history[move] += clampedBonus - static_cast<int16_t>(static_cast<int32_t>(history[move]) * static_cast<int32_t>(std::abs(clampedBonus)) / INT16_MAX);
    }
}