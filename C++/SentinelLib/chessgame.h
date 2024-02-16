#pragma once

#include "chessboard.h"
#include "chessplayer.h"
#include "chessturn.h"

#include <fstream>
#include <iostream>
#include <mutex>
#include <map>

namespace chess
{

    class chessgame
    {
    public:
        chessgame();

        friend class chessplayer;
        friend class chesslobby;

        // Getters for players note these
        // are accessible in the event as well
        error_e save_game(std::string filename);
        std::string save_xfen();

        chessboard board();
        chessturn_s last_turn();
        game_state_e state();
        chessplayertype_e white_type();
        chessplayertype_e black_type();

        color_e turn_color();
        color_e win_color();
        int turnno();

        // Any Player
        error_e forfeit(color_e col);
        error_e move(color_e col, move_s m0);
        error_e move(color_e col, coord_s p0, coord_s p1, piece_e promote = p_none);
        std::vector<move_s> possible_moves(color_e col);
        game_state_e is_game_over(color_e col, move_s &m);

        // Check?
        bool check_state(color_e col);
        std::string check_state();

    protected:
        // Administrative / Lobby
        error_e remove_piece(coord_s p0);
        error_e add_piece(coord_s p0, chesspiece &p1);
        error_e new_game(std::map<color_e, std::shared_ptr<chessplayer>> players, int skill);
        error_e load_game(std::string filename);
        error_e rewind_game(int move_no);
        error_e load_xfen(std::string contents);

        error_e _register(std::shared_ptr<chessplayer>, color_e);
        error_e _unregister(color_e);
        error_e end_game(game_state_e, color_e);

    private:
        // Signallers
        std::shared_ptr<chessplayer> _default_computer_player(int skill);
        void signal_refresh_board();
        void signal_on_consider(move_s &, color_e, int pct = -1);
        void signal_on_move(move_s &, color_e);
        void signal_on_turn();
        void signal_on_end();

        game_state_e m_state;
        std::vector<chessturn_s> m_turn;

        chessplayertype_e m_white_type;
        chessplayertype_e m_black_type;

        color_e m_win_color;

        chessboard m_board;

        std::mutex m_mutex;
        std::map<color_e, std::shared_ptr<chessplayer>> m_players;
    };

}