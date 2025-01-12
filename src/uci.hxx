#include <iostream>
#include <chess.hpp>
#include <cstdint>
#include "settings.hxx"
#include "board.hxx"

namespace Engine::UCI
{
    void parsePosition(Engine::Board& board, const std::string& line);

    uint32_t parseGo(const Engine::Board& board, const std::string& line);
    
    uint32_t parseEach(Engine::Board& board);

    void parseOption(const std::string& line);

    void announceMove(chess::Move& move);

    void announceInfo(chess::Move PV[256], const int32_t depth, const int32_t selDepth, const int32_t score, const uint32_t nodes, const uint32_t elapsedMilliseconds);
}