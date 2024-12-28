#include <chess.hpp>
#include "helpers.hxx"

namespace Engine::PST
{
    /* values from Rofchade, found on chessprogramming.org: https://www.chessprogramming.org/PeSTO%27s_Evaluation_Function */
    
    class PieceSquareTable
    {
    private:
        int middlegameTables[6][64];
        int endgameTables[6][64];

    public:
        PieceSquareTable();

        inline int32_t get_value_middlegame(chess::Color color, chess::PieceType pieceType, uint8_t square)
        {
            if (color == chess::Color::BLACK) square = flip(square);
            return middlegameTables[static_cast<int>(pieceType)][square];
        }

        inline int32_t get_value_endgame(chess::Color color, chess::PieceType pieceType, uint8_t square)
        {
            if (color == chess::Color::BLACK) square = flip(square);
            return endgameTables[static_cast<int>(pieceType)][square];
        }
    };
}