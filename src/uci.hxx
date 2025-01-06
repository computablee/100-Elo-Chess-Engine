#include <iostream>
#include <chess.hpp>
#include <cstdint>
#include "settings.hxx"

namespace Engine::UCI
{
    void parsePosition(chess::Board& board, const std::string& line);

    uint32_t parseGo(const chess::Board& board, std::string input);

    void parseStart(Engine::Settings& settings);
    
    uint32_t parseEach(chess::Board& board);

    void announceMove(chess::Move& move);

    void announceInfo(chess::Move PV[256], const int32_t depth, const int32_t score, const uint32_t nodes);
}