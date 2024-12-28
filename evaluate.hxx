#include <chess.hpp>
#include "helpers.hxx"
#include "settings.hxx"

namespace Engine::Evaluate
{
    int32_t heuristic(const chess::Board& board, const int& distanceToMaxDepth, Engine::Helpers::GameOverResult gameover, const Engine::Settings& settings);

    int32_t calculateMaterial(const chess::Board& board);
}