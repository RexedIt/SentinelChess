#pragma once

#include "chesscommon.h"
#include "chessmove.h"
#include "chessmeta.h"

namespace chess
{

    class chesspgn : public chessmeta
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

        error_e write_moves(std::vector<chessmove> &move_vec);
        error_e load(std::string filename, std::string &errextra);
        error_e save(std::string filename);

    protected:
        virtual void reset_tags();

    private:
        error_e read_tag(std::string line);
        std::string m_moves_str;
        std::string m_xfen;
        std::vector<chessmove> m_moves;
    };

    // Shared with openings
    error_e read_pgn_moves(std::string moves_str, std::vector<chessmove> &move_vec, std::string &xfen, std::string &errextra);

}