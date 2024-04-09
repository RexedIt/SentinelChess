#include "chesspuzzleplayer.h"

#include <chrono>
#include <thread>

namespace chess
{

    chesspuzzleplayer::chesspuzzleplayer()
    {
        m_playertype = t_puzzle;
        m_listenertype = cl_computer;
        m_name = "Puzzle";
        m_cancel = false;
        m_thread_running = false;
    }

    chesspuzzleplayer::chesspuzzleplayer(color_e color, std::string name, int32_t skill)
    {
        m_color = color;
        m_playertype = t_puzzle;
        m_listenertype = cl_computer;
        m_name = name;
        m_skill = skill;
        m_cancel = false;
        m_thread_running = false;
    }

    chesspuzzleplayer::~chesspuzzleplayer()
    {
        cancel_execution();
    }

    void chesspuzzleplayer::cancel_execution()
    {
        while (m_thread_running)
        {
            m_cancel = true;
            std::this_thread::sleep_for(std::chrono::milliseconds(25));
        }
    }

    void chesspuzzleplayer::signal_on_turn(int16_t turn_no, chessmove m, bool check, chessboard &board, color_e color, game_state_e game_state, color_e win_color, int32_t wt, int32_t bt)
    {
        // This is where we will determine game state, move, or forfeit.
        // move:
        game_state_e cur_state = mp_game->state();
        if ((!m_thread_running) && (cur_state == play_e) && (color == m_color))
        {
            m_board = board;
            m_moves = mp_game->player_moves(m_color);
            m_thread_running = true;
            std::thread background(&chesspuzzleplayer::puzzle_move, this, std::ref(m_board), std::ref(m_moves), turn_no);
            m_thread_id = background.get_id();
            background.detach();
        }
    }

    void chesspuzzleplayer::signal_on_state(game_state_e game_state, color_e win_color)
    {
        // if (game_state != play_e)
        //     cancel_execution();
    }

    void chesspuzzleplayer::stop_listening()
    {
        if (mp_game)
            mp_game->unlisten(id());
    }

    error_e chesspuzzleplayer::puzzle_move(chessboard &board, std::map<int16_t, chessmove> &moves, int16_t turn_no)
    {
        error_e err = e_none;
        if (moves.count(turn_no + 1) == 0)
            err = puzzle_solved(); // out of moves, player wins
        else
            err = move(moves[turn_no + 1]);
        m_thread_running = false;
        return err;
    }

    error_e chesspuzzleplayer::puzzle_solved()
    {
        std::lock_guard<std::mutex> guard(chessplayer::m_mutex);
        if (mp_game == NULL)
            return e_no_game;
        return mp_game->puzzle_solved(m_color);
    }

}