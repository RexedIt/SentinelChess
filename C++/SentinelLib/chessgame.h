#pragma once

#include "chessboard.h"
#include "chessturn.h"

#include <fstream>
#include <iostream>
#include <thread>

namespace chess
{
    typedef void (*draw_board_callback)(int, chessboard &);
    typedef void (*draw_move_callback)(int, move_s &, color_e);
    typedef piece_e (*request_promote_callback)();
    typedef void (*game_end_callback)(game_state_e, color_e);
    typedef void (*computer_moved_callback)(int, chessturn_s &);

    class chessgame
    {
    public:
        chessgame();
        void new_game(color_e user_color, int level);
        error_e load_game(std::string filename);
        error_e save_game(std::string filename);

        error_e load_xfen(std::string contents);
        std::string save_xfen();

        chessboard board();
        game_state_e state();
        color_e user_color();
        color_e turn_color();
        color_e win_color();

        int level();

        error_e computer_move(color_e col);
        error_e computer_move_async(color_e col);
        void computer_move_cancel();
        bool computer_moving();
        error_e forfeit();

        error_e user_move(color_e col, move_s m0);
        error_e user_move(color_e col, coord_s p0, coord_s p1, piece_e promote = p_none);
        std::vector<move_s> possible_moves(color_e col);

        error_e suggest_move(move_s m);
        error_e suggest_move(coord_s p0, coord_s p1, int cx = -1, bool en_passant = false, piece_e promote = p_none);
        error_e rewind_game(int move_no);
        error_e remove_piece(coord_s p0);
        error_e add_piece(coord_s p0, chesspiece &p1);

        // Check?
        bool check_state(color_e col);
        std::string check_state();
        std::string cache_stats();

        void set_callbacks(draw_board_callback _draw_board, game_end_callback _game_end, computer_moved_callback _computer_moved,
                           draw_move_callback _draw_move = NULL, request_promote_callback _request_promote = NULL,
                           thinking_callback _thinking = NULL, traces_callback _traces = NULL);

        chessturn_s last_turn();
        int turnno();

    private:
        game_state_e m_state;
        std::vector<chessturn_s> m_turn;
        color_e m_user_color;
        color_e m_win_color;
        std::thread::id m_thread_id;
        bool m_thread_running;

        int m_level;
        int m_trace_level;
        chessboard m_board;

        void draw_board(int n, chessboard &b);
        void game_over(game_state_e state, color_e win_color);
        void draw_move(int n, move_s &m, color_e c);
        void draw_turn(int n, chessboard &b, move_s &m, color_e c);
        void computer_moved(int n, chessturn_s &t);

        void computer_move_background(color_e col);
        game_state_e is_game_over(color_e col, move_s &m);

        draw_board_callback mp_cb_draw_board;
        game_end_callback mp_cb_game_end;
        computer_moved_callback mp_cb_computer_moved;
        draw_move_callback mp_cb_draw_move;
        request_promote_callback mp_cb_request_promote;
    };

}