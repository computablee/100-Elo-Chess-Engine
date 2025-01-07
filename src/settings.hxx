#pragma once
#include <string>
#include <cstdint>

#define BEST_EVAL 1000000
#define WORST_EVAL (-1000000)

namespace Engine
{
    class Settings
    {
    private:
        const std::string engine_name;
        const std::string engine_author;

    public:
        Settings();

        inline std::string get_engine_name() const { return this->engine_name; }
        inline std::string get_engine_author() const { return this->engine_author; }
    };
}