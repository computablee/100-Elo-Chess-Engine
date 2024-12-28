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
        ttEntry* transpositionTable;
        uint32_t elements;

    public:
        Table(uint32_t bytes);

        inline ttEntry get_entry(chess::Board& board)
        {
            return this->transpositionTable[Engine::Helpers::reduce(board.hash(), this->elements)];
        }

        inline void set_entry(chess::Board& board, ttEntry& entry)
        {
            this->transpositionTable[Engine::Helpers::reduce(board.hash(), this->elements)] = entry;
        }
    };
}