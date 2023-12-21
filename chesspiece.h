#pragma once

#include <string>
#include <vector>

namespace chess
{

    class chesspiece
    {
    public:
        chesspiece();
        chesspiece(unsigned char c);
        chesspiece(std::string code0);

        void possible_moves(std::vector<move_s> &possible, short y0, short x0, unsigned char (&cells)[8][8]);
        void update_kill_bits(short y0, short x0, unsigned char (&cells)[8][8]);

        piece_e ptype = p_none;
        color_e color = c_none;
        std::string abbr = " ";
        std::string name;
        std::string code = "  ";
        unsigned char value = 0;

    private:
        void calc_direction_moves(std::vector<move_s> &possible, short y0, short x0, short dy, short dx, short max, unsigned char (&cells)[8][8]);
        void calc_single_move(std::vector<move_s> &possible, short y0, short x0, short x1, short y1, unsigned char (&cells)[8][8]);
        void calc_king_move(std::vector<move_s> &possible, short y0, short x0, short x1, short y1, unsigned char (&cells)[8][8]);
        void pawn_moves(std::vector<move_s> &possible, short y0, short x0, unsigned char (&cells)[8][8]);
        void bishop_moves(std::vector<move_s> &possible, short y0, short x0, unsigned char (&cells)[8][8]);
        void knight_moves(std::vector<move_s> &possible, short y0, short x0, unsigned char (&cells)[8][8]);
        void rook_moves(std::vector<move_s> &possible, short y0, short x0, unsigned char (&cells)[8][8]);
        void queen_moves(std::vector<move_s> &possible, short y0, short x0, unsigned char (&cells)[8][8]);
        void king_moves(std::vector<move_s> &possible, short y0, short x0, unsigned char (&cells)[8][8]);
        void calc_direction_kills(short y0, short x0, short dy, short dx, short max, unsigned char (&cells)[8][8]);
        void calc_single_kill(short y0, short x0, short x1, short y1, unsigned char (&cells)[8][8]);
        void pawn_kills(short y0, short x0, unsigned char (&cells)[8][8]);
        void bishop_kills(short y0, short x0, unsigned char (&cells)[8][8]);
        void knight_kills(short y0, short x0, unsigned char (&cells)[8][8]);
        void rook_kills(short y0, short x0, unsigned char (&cells)[8][8]);
        void queen_kills(short y0, short x0, unsigned char (&cells)[8][8]);
        void king_kills(short y0, short x0, unsigned char (&cells)[8][8]);
    };

}