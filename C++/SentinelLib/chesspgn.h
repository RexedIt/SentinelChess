#pragma once

#include <map>
#include "chesscommon.h"
#include "chessmove.h"

namespace chess
{

    class chesspgn
    {
    public:
        chesspgn();

        std::string event();
        std::string white();
        std::string black();
        int whiteelo();
        int blackelo();
        std::string eco();
        std::string opening();
        std::string moves_str();
        std::string xfen();

        std::vector<chessmove> moves();
        color_e win_color();
        game_state_e game_state();
        void set_final_state(game_state_e, color_e);

        std::string operator[](const char *);
        bool remove_tag(std::string key);
        bool write_tag(std::string key, std::string value);
        bool write_tag(std::string key, int value);

        error_e write_moves(std::vector<chessmove> &move_vec);
        error_e load(std::string filename, std::string &errextra);
        error_e save(std::string filename);

    private:
        void reset_tags();
        error_e read_tag(std::string line);
        std::string tag(std::string key);

        std::vector<std::pair<std::string, std::string>> m_tags;
        std::string m_moves_str;
        std::string m_xfen;
        std::vector<chessmove> m_moves;
    };

    // Shared with openings
    error_e read_pgn_moves(std::string moves_str, std::vector<chessmove> &move_vec, std::string &xfen, std::string &errextra);

}