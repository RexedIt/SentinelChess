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
        if ((pu == "HUMAN") || (pu == "H"))
            return t_human;
        if ((pu == "COMPUTER") || (pu == "C"))
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
        mp_game = NULL;
    }

    chessplayer::chessplayer(color_e color, chessplayerdata data)
    {
        m_data = data;
        m_color = color;
        mp_game = NULL;
    }

    chessplayer::chessplayer(color_e color, std::string name, int32_t skill, chessplayertype_e ptype)
    {
        m_color = color;
        m_data.username = name;
        m_data.elo = skill;
        m_data.ptype = ptype;
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
        return m_data.ptype;
    }

    std::string chessplayer::playername()
    {
        return m_data.username;
    }

    int32_t chessplayer::playerskill()
    {
        return m_data.elo;
    }

    chessplayerdata chessplayer::playerdata()
    {
        return m_data;
    }

    bool chessplayer::is(color_e c)
    {
        return c == m_color;
    }

    error_e chessplayer::forfeit()
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        if (mp_game == NULL)
            return e_no_game;
        return mp_game->forfeit(m_color);
    }

    error_e chessplayer::move(chessmove m0)
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        if (mp_game == NULL)
            return e_no_game;
        return mp_game->move(m_color, m0);
    }

    error_e chessplayer::move(coord_s p0, coord_s p1, piece_e promote)
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        if (mp_game == NULL)
            return e_no_game;
        return mp_game->move(m_color, p0, p1, promote);
    }

    error_e chessplayer::move(std::string s)
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        if (mp_game == NULL)
            return e_no_game;
        return mp_game->move(m_color, s);
    }

    chessboard chessplayer::board()
    {
        return mp_game->board();
    }

    std::vector<chessmove> chessplayer::possible_moves()
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        std::vector<chessmove> p;
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

    error_e chessplayer::consider(chessmove &m, int8_t p)
    {
        if (mp_game != NULL)
        {
            mp_game->consider(m, m_color, p);
            return e_none;
        }
        return e_no_game;
    }

}