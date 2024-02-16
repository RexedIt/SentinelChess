#include "chessplayer.h"

namespace chess
{

    std::vector<std::string> playertypes()
    {
        std::vector<std::string> ret;
        ret.push_back("Human");
        ret.push_back("Computer");
        return ret;
    }

    chessplayertype_e playertypefromstring(std::string p)
    {
        std::string pu = uppercase(p);
        if (pu == "HUMAN")
            return t_human;
        if (pu == "COMPUTER")
            return t_computer;
        return t_none;
    }

    std::string playertypetostring(chessplayertype_e p)
    {
        switch (p)
        {
        case t_human:
            return "Human";
        case t_computer:
            return "Computer";
        default:
            return "None";
        }
    }

    chessplayer::chessplayer()
    {
        m_color = c_none;
        m_playertype = t_none;
        mp_game = NULL;
        m_skill = 0;
    }

    chessplayer::chessplayer(std::string name, int32_t skill)
    {
        m_name = name;
        m_skill = skill;
        m_playertype = t_none;
        mp_game = NULL;
    }

    chessplayer::~chessplayer()
    {
    }

    color_e chessplayer::playercolor()
    {
        return m_color;
    }

    void chessplayer::set_game(std::shared_ptr<chessgame> p_game)
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        mp_game = p_game;
    }

    chessplayertype_e chessplayer::playertype()
    {
        return m_playertype;
    }

    std::string chessplayer::playername()
    {
        return m_name;
    }

    int32_t chessplayer::playerskill()
    {
        return m_skill;
    }

    bool chessplayer::is(color_e c)
    {
        return c == m_color;
    }

    error_e chessplayer::forfeit()
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        if (mp_game != NULL)
            return e_no_game;
        return mp_game->forfeit(m_color);
    }

    error_e chessplayer::move(move_s m0)
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        if (mp_game != NULL)
            return e_no_game;
        return mp_game->move(m_color, m0);
    }

    error_e chessplayer::move(coord_s p0, coord_s p1, piece_e promote)
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        if (mp_game != NULL)
            return e_no_game;
        return mp_game->move(m_color, p0, p1, promote);
    }

    chessboard chessplayer::board()
    {
        chessboard b;
        if (mp_game != NULL)
            b.copy(mp_game->board());
        return b;
    }

    std::vector<move_s> chessplayer::possible_moves()
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        std::vector<move_s> p;
        if (mp_game != NULL)
            p = mp_game->possible_moves(m_color);
        return p;
    }

    error_e chessplayer::chat(std::string msg)
    {
        if (mp_game != NULL)
        {
            mp_game->chat(msg, m_color);
            return e_none;
        }
        return e_no_game;
    }

}