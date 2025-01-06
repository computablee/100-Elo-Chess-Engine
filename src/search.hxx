#pragma once
#include <list>
#include <chess.hpp>
#include <cstdint>
#include <climits>
#include <utility>
#include <chrono>
#include "settings.hxx"
#include "helpers.hxx"

using namespace Engine::Helpers;

namespace Engine::Search
{
    inline int32_t count = 0;
    inline uint8_t maxPly = 0;

    class TimeOut : std::exception { };

    enum class NodeType { NonPV, PV };

    inline std::chrono::steady_clock::time_point begin;

    template <NodeType nodeType>
    int32_t search(chess::Board& board, int32_t alpha, int32_t beta, uint8_t depth, const uint8_t ply, chess::Move PV[256]);

    void updatePV(chess::Move PV[256], const chess::Move& move);

    void updatePV(chess::Move PVup[256], const chess::Move PVdown[256], const chess::Move& move);

    chess::Move iterativeDeepening(chess::Board& board);

    int32_t quiescence(chess::Board& board, int32_t alpha, int32_t beta, const uint8_t ply);
}