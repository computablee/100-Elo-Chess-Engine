#pragma once
#include <string>
#include <cstdint>

namespace Engine
{
    class Settings
    {
    private:
        const std::string engine_name;
        const std::string engine_author;
        const bool use_tt;
        const int32_t best_eval;
        const int32_t worst_eval;

    public:
        Settings();

        inline std::string get_engine_name() const { return this->engine_name; }
        inline std::string get_engine_author() const { return this->engine_author; }
        inline bool get_use_tt() const { return this->use_tt; }
        inline int32_t get_best_eval() const { return this->best_eval; }
        inline int32_t get_worst_eval() const { return this->worst_eval; }
    };
}