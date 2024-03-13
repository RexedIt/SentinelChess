#pragma once

#include <string>
#include <vector>

#include "chesscommon.h"

namespace chess
{

    class chesspiece
    {
    public:
        chesspiece();
        chesspiece(unsigned char c);
        chesspiece(char code);
        chesspiece(color_e c, piece_e p);
        chesspiece(const chesspiece &);
        void copy(const chesspiece &);
        void operator=(const chesspiece &);

        void possible_moves(std::vector<chessmove> &possible, coord_s p0, unsigned char (&cells)[8][8], unsigned char castled_left, unsigned char castled_right, coord_s ep);
        void update_kill_bits(coord_s p0, unsigned char (&cells)[8][8], int &kc);

        piece_e ptype;
        color_e color;
        char abbr;
        std::string name;
        unsigned char value;
        unsigned char kill_mask;
        unsigned char enemy_kill_mask;

    private:
        void calc_direction_moves(std::vector<chessmove> &possible, coord_s p0, int8_t dy, int8_t dx, int8_t max, unsigned char (&cells)[8][8]);
        void calc_single_move(std::vector<chessmove> &possible, coord_s p0, int8_t dy, int8_t dx, unsigned char (&cells)[8][8]);
        void calc_king_move(std::vector<chessmove> &possible, coord_s p0, int8_t dy, int8_t dx, unsigned char (&cells)[8][8]);
        void pawn_moves(std::vector<chessmove> &possible, coord_s p0, unsigned char (&cells)[8][8], coord_s ep);
        void bishop_moves(std::vector<chessmove> &possible, coord_s p0, unsigned char (&cells)[8][8]);
        void knight_moves(std::vector<chessmove> &possible, coord_s p0, unsigned char (&cells)[8][8]);
        void rook_moves(std::vector<chessmove> &possible, coord_s p0, unsigned char (&cells)[8][8]);
        void queen_moves(std::vector<chessmove> &possible, coord_s p0, unsigned char (&cells)[8][8]);
        void king_moves(std::vector<chessmove> &possible, coord_s p0, unsigned char (&cells)[8][8], unsigned char castled_left, unsigned char castled_right);
        void calc_direction_kills(coord_s p0, int8_t dy, int8_t dx, int8_t max, unsigned char (&cells)[8][8], int &kc);
        void calc_single_kill(coord_s p0, int8_t xdx1, int8_t dy, unsigned char (&cells)[8][8], int &kc);
        void pawn_kills(coord_s p0, unsigned char (&cells)[8][8], int &kc);
        void bishop_kills(coord_s p0, unsigned char (&cells)[8][8], int &kc);
        void knight_kills(coord_s p0, unsigned char (&cells)[8][8], int &kc);
        void rook_kills(coord_s p0, unsigned char (&cells)[8][8], int &kc);
        void queen_kills(coord_s p0, unsigned char (&cells)[8][8], int &kc);
        void king_kills(coord_s p0, unsigned char (&cells)[8][8], int &kc);
        bool castle_path(coord_s p0, int8_t y1, int8_t x1, unsigned char (&cells)[8][8]);
        void possible_pawn_move(std::vector<chessmove> &possible, coord_s p0, int8_t dy, int8_t dx);
    };

}