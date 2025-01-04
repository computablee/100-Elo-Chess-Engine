#include <chess.hpp>
#include "helpers.hxx"

namespace Engine::PST
{
    /* values from Rofchade, found on chessprogramming.org: https://www.chessprogramming.org/PeSTO%27s_Evaluation_Function */
    
    class PieceSquareTable
    {
    private:
        int32_t middlegameTables[6][64];
        int32_t endgameTables[6][64];

    public:
        PieceSquareTable();

        constexpr inline __attribute__((always_inline)) int32_t get_value_middlegame(int32_t color, int32_t pieceType, uint8_t square)
        {
            if (color) square = Engine::Helpers::flip(square);
            return middlegameTables[pieceType][square];
        }

        constexpr inline __attribute__((always_inline)) int32_t get_value_endgame(int32_t color, int32_t pieceType, uint8_t square)
        {
            if (color) square = Engine::Helpers::flip(square);
            return endgameTables[pieceType][square];
        }
    };
}