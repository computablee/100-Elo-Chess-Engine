#include "uci.hxx"

using namespace chess;

namespace Engine::UCI
{
    uint32_t parseGo(const Board& board, std::string input)
    {
        int wtime, btime, winc, binc;
        std::stringstream stream;
        stream.str(input);
        std::string part;
        stream >> part;
        assert(part == "go");
        stream >> part;
        assert(part == "wtime");
        stream >> wtime;
        stream >> part;
        assert(part == "btime");
        stream >> btime;
        stream >> part;
        assert(part == "winc");
        stream >> winc;
        stream >> part;
        assert(part == "binc");
        stream >> binc;

        if (board.sideToMove() == Color::WHITE)
            return wtime / 20 + winc / 2 - 10;
        else
            return btime / 20 + binc / 2 - 10;
    }

    void parsePosition(Board& board, const std::string& line)
    {
        std::stringstream stream;
        stream.str(line);
        std::string part;
        std::string start;
        stream >> part;
        assert(part == "position");
        stream >> part;
        if (part == "startpos")
            start = chess::constants::STARTPOS;
        else if (part == "fen")
        {
            stream >> part;
            start = part;
        }
        else assert(false);

        board = Board(start);

        stream >> part;
        if (part == "moves")
        {
            stream >> part;
            while (!stream.eof())
            {
                board.makeMove(uci::uciToMove(board, part));
                stream >> part;
            }

            board.makeMove(uci::uciToMove(board, part));
        }
    }

    void parseStart(Engine::Settings& settings)
    {
        std::string line;
        auto progress = false;

        while (!progress)
        {
            std::getline(std::cin, line);

            if (line == "uci")
            {
                std::cout << "id name " << settings.get_engine_name() << std::endl;
                std::cout << "id author " << settings.get_engine_author() << std::endl;
                std::cout << "uciok" << std::endl;
            }
            else if (line == "isready")
            {
                std::cout << "readyok" << std::endl;
                progress = true;
            }
        }
    }

    uint32_t parseEach(Board& board)
    {
        std::string line;
        auto progress = false;

        while (!progress)
        {
            std::getline(std::cin, line);

            if (line.substr(0, 8) == "position")
            {
                parsePosition(board, line);
            }
            else if (line == "isready")
            {
                std::cout << "readyok" << std::endl;
            }
            else if (line.substr(0, 2) == "go")
            {
                progress = true;
                return parseGo(board, line);
            }
            else if (line == "quit")
            {
                exit(0);
            }
        }

        return 0;
    }

    void announceMove(chess::Move& move)
    {
        std::cout << "bestmove " << uci::moveToUci(move) << std::endl;
    }
}