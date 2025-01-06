#include <chess.hpp>
#include <string>

#include "settings.hxx"
#include "uci.hxx"
#include "search.hxx"

using namespace chess;
using namespace Engine;
using namespace Engine::UCI;
using namespace Engine::Search;

Settings settings;

uint32_t milliseconds_to_think;

int main()
{
    Board board;

    parseStart(settings);

    while (true)
    {
        milliseconds_to_think = parseEach(board);
        auto bestmove = iterativeDeepening(board);
        announceMove(bestmove);
        board.makeMove(bestmove);
    }

    return 0;
}