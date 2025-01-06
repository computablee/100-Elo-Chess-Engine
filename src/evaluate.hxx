#include <chess.hpp>
#include "helpers.hxx"
#include "settings.hxx"

namespace Engine::Evaluate
{
    int32_t heuristic(const chess::Board& board, const int ply, Engine::Helpers::GameOverResult gameover);

    int32_t calculateMaterial(const chess::Board& board);
}