#include "chesspuzzleplayer.h"

namespace chess
{

    chesspuzzleplayer::chesspuzzleplayer()
    {
        m_playertype = t_puzzle;
        m_listenertype = cl_computer;
        m_name = "Puzzle";
    }

    chesspuzzleplayer::chesspuzzleplayer(color_e color, std::string name, int32_t skill)
    {
        m_color = color;
        m_playertype = t_puzzle;
        m_listenertype = cl_computer;
        m_name = name;
        m_skill = skill;
    }

    chesspuzzleplayer::~chesspuzzleplayer()
    {
    }

    void chesspuzzleplayer::signal_on_turn(int16_t turn_no, chessmove m, bool check, chessboard &board, color_e color, game_state_e game_state, color_e win_color, int32_t wt, int32_t bt)
    {
        game_state_e cur_state = mp_game->state();
        if ((cur_state == play_e) && (color == m_color))
        {
            std::map<int16_t, chessmove> moves = mp_game->player_moves(m_color);
            error_e err = e_none;
            if (moves.count(turn_no)==0)
                err = forfeit();
            else
                err = move(moves[turn_no]);
            if (err != e_none)
                throw;
        }
    }

}