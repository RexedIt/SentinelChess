#pragma once

#include "chesscommon.h"

namespace chess
{

    class chessmove
    {

    public:
        chessmove();
        chessmove(coord_s _p0, coord_s _p1, int8_t _cx = -1, bool _en_passant = false);
        chessmove(coord_s _p0, coord_s _p1, piece_e _promote);

        std::string to_string();
        bool is_valid();
        void invalidate();
        bool matches(const chessmove &);

        // since we use this like a struct I don't want the overhead of getters and setters for
        // public access
        coord_s p0;
        coord_s p1;
        int8_t cx;
        piece_e promote;
        bool en_passant;
        bool check;
        bool mate;
        error_e error;
    };

    // static and utility type functions
    class chessboard;

    bool contains_move(std::vector<chessmove> possible_moves, chessmove &m, bool inherit = false);
    bool contains_move_dest(std::vector<chessmove> possible_moves, chessmove &m);
    std::string move_str(chessmove m);
    error_e str_move(std::string s, chessmove &m);
    error_e str_move(std::string s, color_e c, chessboard &b, chessmove &m);
    chessmove new_move(int8_t y0, int8_t x0, int8_t y1, int8_t x1);
    chessmove new_move(coord_s p0, coord_s p1, int8_t cx = -1, bool en_passant = false);
    chessmove new_move(coord_s p0, coord_s p1, piece_e promote);

}