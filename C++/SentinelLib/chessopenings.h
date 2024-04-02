#pragma once

#include "chesscommon.h"
#include "chessmove.h"

#include <vector>
#include <memory>
#include <map>
#include <set>

namespace chess
{
    class chessopening
    {
    public:
        chessopening();
        chessopening(const chessopening &);
        chessopening(std::string eco, std::string title, std::vector<chessmove> moves, uint32_t hash = 0);
        void copy(const chessopening &);
        void operator=(const chessopening &);
        bool matches(std::vector<chessmove> filter);
        bool startswith(std::vector<chessmove> filter);
        bool get_move(size_t index, chessmove &m);

        std::string eco;
        std::string title;
        std::vector<chessmove> moves;
        uint32_t hash();

    private:
        uint32_t m_hash;
    };

    class chessecodb
    {
    public:
        chessecodb();
        ~chessecodb();

        error_e load_scid_eco(std::string filename);
        error_e load_binary(std::string filename);
        error_e save_binary(std::string filename);

        error_e chessopenings(std::vector<chessopening> &openings);
        error_e preferredecos(color_e col, std::vector<std::string> &ecos);

    private:
        void initialize();
        std::vector<chessopening> m_openings;
        std::map<color_e, std::vector<std::string>> m_preferred;
    };

    // static calls dependent on instantiation of eco db
    error_e get_chessopenings(std::vector<chessopening> &openings);
    error_e get_preferredecos(color_e col, std::vector<std::string> &ecos);
    error_e load_scid_line(std::string line, chessopening &open, std::string &errextra);

    class chessopenfilter
    {
    public:
        chessopenfilter();
        ~chessopenfilter();

        void initialize();
        void reset();
        error_e narrow(std::vector<chessmove> &filter);
        std::string eco();
        std::string title();
        std::vector<chessopening> possible_openings();
        size_t possible_opening_count();
        error_e next_opening_moves(color_e col, std::string eco, std::vector<chessmove> &possible);

    private:
        std::vector<chessopening> m_openings;
        std::vector<chessopening> m_filtered;
        std::string m_eco;
        std::string m_title;
        std::vector<chessmove> m_last;
        int m_last_match_size;

        void find_best_opening_match();
    };
}