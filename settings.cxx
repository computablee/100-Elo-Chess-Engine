#include "settings.hxx"

using namespace Engine;

Settings::Settings() :
    engine_name("100 Elo Chess Engine (Refactored)"),
    engine_author("Phillip Allen Lane"),
    use_tt(true),
    best_eval(1000000),
    worst_eval(-1000000)
{ }