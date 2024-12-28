#pragma once
#include <list>
#include <chess.hpp>
#include <cstdint>
#include <climits>
#include <utility>
#include <chrono>
#include "settings.hxx"
#include "helpers.hxx"

using namespace Engine::Helpers;

namespace Engine::Search
{
    inline int32_t count = 0;
    inline int32_t maxPly = 0;

    class TimeOut : std::exception { };

    class Sequence
    {
    private:
        int32_t evaluation;
        std::list<chess::Move> sequence;

    public:
        inline Sequence(int32_t evaluation, std::list<chess::Move>&& sequence) : evaluation(evaluation), sequence(std::move(sequence)) { }

        inline Sequence(int32_t evaluation) : evaluation(evaluation), sequence(std::list<chess::Move>()) { }

        inline Sequence(int32_t evaluation, chess::Move move, Sequence&& sequence) : evaluation(evaluation), sequence(std::move(sequence).get_sequence()) { this->sequence.push_front(move); }

        inline Sequence() : evaluation(INT_MIN), sequence(std::list<chess::Move>()) { }

        inline Sequence(Sequence& sequence) = delete;

        inline Sequence(Sequence&& sequence) : evaluation(sequence.get_evaluation()), sequence(std::move(sequence).get_sequence()) { }

        inline int32_t get_evaluation() { return this->evaluation; }

        inline std::list<chess::Move>&& get_sequence() && { return std::move(this->sequence); }

        inline chess::Move get_first_move() { return this->sequence.front(); }

        inline void operator=(Sequence& sequence) { this->evaluation = sequence.evaluation, this->sequence = std::move(sequence.sequence); }

        inline void operator=(Sequence&& sequence) { this->evaluation = sequence.evaluation, this->sequence = std::move(sequence.sequence); }
    };

    inline std::chrono::steady_clock::time_point begin;

    Sequence search(chess::Board& board, int32_t alpha, int32_t beta, const int depth, const int maxDepth, const int color, const Settings& settings);

    chess::Move iterativeDeepening(chess::Board& board, const Settings& settings);

    int32_t quiescence(chess::Board& board, int32_t alpha, int32_t beta, const int depth, const int maxDepth, const int color, const Settings& settings);
}