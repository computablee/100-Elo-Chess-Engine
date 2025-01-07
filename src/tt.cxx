#include "tt.hxx"

using namespace Engine::TranspositionTable;
using namespace chess;

Table::Table(uint64_t bytes) :
    elements(bytes / sizeof(ttEntry)),
    transpositionTable(new ttEntry[this->elements])
{ }