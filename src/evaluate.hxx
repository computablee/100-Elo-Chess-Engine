#include <chess.hpp>
#include "helpers.hxx"
#include "settings.hxx"
#include "board.hxx"

namespace Engine::Evaluate
{
    int32_t heuristic(const Engine::Board& board, const int ply, Engine::Helpers::GameOverResult gameover);

    int32_t calculateMaterial(const chess::Board& board);
}