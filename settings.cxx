#include "settings.hxx"

using namespace Engine;

Settings::Settings() :
    engine_name("100 Elo Chess Engine"),
    engine_author("Phillip Allen Lane"),
    use_tt(true),
    best_eval(1000000),
    worst_eval(-1000000)
{ }

/*Settings::Settings(Settings& settings) :
    engine_name(settings.get_engine_name()),
    engine_author(settings.get_engine_author()),
    use_tt(settings.get_use_tt()),
    best_eval(settings.get_best_eval()),
    worst_eval(settings.get_worst_eval())
{ }

Settings::Settings(const Settings& settings) :
    engine_name(settings.get_engine_name()),
    engine_author(settings.get_engine_author()),
    use_tt(settings.get_use_tt()),
    best_eval(settings.get_best_eval()),
    worst_eval(settings.get_worst_eval())
{ }*/