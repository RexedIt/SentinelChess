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

        void possible_moves(std::vector<move_s> &possible, int8_t y0, int8_t x0, unsigned char (&cells)[8][8], unsigned char castled_left, unsigned char castled_right, int8_t ep_y, int8_t ep_x);
        void update_kill_bits(int8_t y0, int8_t x0, unsigned char (&cells)[8][8]);

        piece_e ptype = p_none;
        color_e color = c_none;
        std::string abbr = " ";
        std::string name;
        std::string code = "  ";
        unsigned char value = 0;

    private:
        void calc_direction_moves(std::vector<move_s> &possible, int8_t y0, int8_t x0, int8_t dy, int8_t dx, int8_t max, unsigned char (&cells)[8][8]);
        void calc_single_move(std::vector<move_s> &possible, int8_t y0, int8_t x0, int8_t x1, int8_t y1, unsigned char (&cells)[8][8]);
        void pawn_moves(std::vector<move_s>& possible, int8_t y0, int8_t x0, unsigned char(&cells)[8][8], int8_t ep_y, int8_t ep_x);
        void bishop_moves(std::vector<move_s> &possible, int8_t y0, int8_t x0, unsigned char (&cells)[8][8]);
        void knight_moves(std::vector<move_s> &possible, int8_t y0, int8_t x0, unsigned char (&cells)[8][8]);
        void rook_moves(std::vector<move_s> &possible, int8_t y0, int8_t x0, unsigned char (&cells)[8][8]);
        void queen_moves(std::vector<move_s> &possible, int8_t y0, int8_t x0, unsigned char (&cells)[8][8]);
        void king_moves(std::vector<move_s> &possible, int8_t y0, int8_t x0, unsigned char (&cells)[8][8], unsigned char castled_left, unsigned char castled_right);
        void calc_direction_kills(int8_t y0, int8_t x0, int8_t dy, int8_t dx, int8_t max, unsigned char (&cells)[8][8]);
        void calc_single_kill(int8_t y0, int8_t x0, int8_t x1, int8_t y1, unsigned char (&cells)[8][8]);
        void pawn_kills(int8_t y0, int8_t x0, unsigned char (&cells)[8][8]);
        void bishop_kills(int8_t y0, int8_t x0, unsigned char (&cells)[8][8]);
        void knight_kills(int8_t y0, int8_t x0, unsigned char (&cells)[8][8]);
        void rook_kills(int8_t y0, int8_t x0, unsigned char (&cells)[8][8]);
        void queen_kills(int8_t y0, int8_t x0, unsigned char (&cells)[8][8]);
        void king_kills(int8_t y0, int8_t x0, unsigned char (&cells)[8][8]);
        bool castle_path(int8_t y0, int8_t x0, int8_t y1, int8_t x1, unsigned char (&cells)[8][8]);
        void possible_pawn_move(std::vector<move_s>& possible, int8_t y0, int8_t x0, int8_t y1, int8_t x1);
    };

}