#include "tt.hxx"

using namespace chess;

namespace Engine::TranspositionTable
{
    Table::Table() :
        elements(MB(1)),
        transpositionTable(new ttEntry[this->elements])
    { }

    void Table::set_size(const uint64_t bytes)
    {
        delete[] this->transpositionTable;
        this->elements = bytes / sizeof(ttEntry);
        this->transpositionTable = new ttEntry[this->elements];
    }

    Table::~Table()
    {
        delete[] this->transpositionTable;
    }
}