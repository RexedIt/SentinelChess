#pragma once

#include "chessboard.h"
#include "chessplayer.h"
#include "chessgamelistener.h"
#include "chessturn.h"
#include "chesspuzzle.h"

#include <fstream>
#include <mutex>
#include <map>

#include "nlohmann/json.hpp"
using namespace nlohmann;

namespace chess
{

    class chessgamelistener;

    class chessgame
    {
    public:
        chessgame();

        friend class chessplayer;
        friend class chesscomputer;
        friend class chessgamelistener;
        friend class chesslobby;

        // Getters for players note these
        // are accessible in the event as well
        std::string save_xfen();

        chessboard board();

        chessturn play_turn();

        game_state_e state();

        color_e turn_color();
        color_e win_color();
        int16_t playmax();
        int16_t playno();

        // Any Player
        error_e forfeit(color_e col);
        error_e move(color_e col, chessmove m0);
        error_e move(color_e col, coord_s p0, coord_s p1, piece_e promote = p_none);
        error_e move(color_e col, std::string s);

        std::vector<chessmove> possible_moves(color_e col);
        std::map<int16_t, chessmove> player_moves(color_e col);

        game_state_e is_game_over(color_e col, chessmove m);
        void clock_remaining(color_e col, int32_t &wt, int32_t &bt);

        // Check?
        bool check_state(color_e col);
        std::string check_state();

        // Administrative / Lobby
        error_e remove_piece(coord_s p0);
        error_e add_piece(coord_s p0, chesspiece &p1);
        error_e load_xfen(std::string contents);
        error_e rewind_game();
        error_e advance_game();
        error_e goto_turn(int turn_no);
        error_e play_game();
        error_e pause_game();

    protected:
        error_e new_game(const chessclock_s &clock);
        error_e load_game(json &j);
        error_e save_game(json &j);
        error_e load_puzzle(chesspuzzle &p);

        error_e listen(std::shared_ptr<chessgamelistener>);
        error_e unlisten(int);
        error_e end_game(game_state_e, color_e);
        error_e chat(std::string, color_e);
        error_e consider(chessmove, color_e, int8_t pct = -1);

    private:
        void set_state(game_state_e g, bool force_notify = false);
        void set_turn_to(int idx);
        void push_new_turn(chessmove m);
        void refresh_board_positions();
        int prev_position_count();
        void add_board_position();
        chessturn new_turn(chessmove);

        // Signallers
        void signal_refresh_board();
        void signal_on_consider(chessmove, color_e, int8_t pct = -1);
        void signal_on_turn();
        void signal_on_state();
        void signal_chat(std::string, color_e);

        game_state_e m_state;
        std::vector<chessturn> m_turn;
        int m_play_pos;
        bool m_puzzle;
        std::string m_opening;

        color_e m_win_color;

        chessboard m_board;

        std::mutex m_mutex;
        std::map<int, std::shared_ptr<chessgamelistener>> mp_listeners;

        // Draw Indicators - Fivefold Repetition
        std::map<uint32_t, int> m_board_positions;
    };

}