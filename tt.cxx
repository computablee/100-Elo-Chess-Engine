#include "tt.hxx"

using namespace Engine::TranspositionTable;
using namespace chess;

Table::Table(uint32_t bytes)
{
    this->elements = bytes / sizeof(ttEntry);
    this->transpositionTable = new ttEntry[this->elements];
}