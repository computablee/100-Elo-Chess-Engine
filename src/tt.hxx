#include <cstdint>
#include <chess.hpp>
#include "helpers.hxx"

namespace Engine::TranspositionTable
{
    enum ttFlag : uint8_t { EXACT, UPPERBOUND, LOWERBOUND };

    struct ttEntry
    {
        uint64_t hash;
        int value;
        ttFlag flag;
        uint8_t depth;
        chess::Move bestMove;
    };

    class Table
    {
    private:
        const uint64_t elements;
        ttEntry* transpositionTable;

    public:
        Table(uint64_t bytes);

        constexpr inline __attribute__((always_inline)) ttEntry& get_entry(const uint64_t hash) const
        {
            return this->transpositionTable[Engine::Helpers::reduce(hash, this->elements)];
        }

        constexpr inline __attribute__((always_inline)) void set_entry(const uint64_t hash, ttEntry& entry)
        {
            this->transpositionTable[Engine::Helpers::reduce(hash, this->elements)] = entry;
        }
    };
}