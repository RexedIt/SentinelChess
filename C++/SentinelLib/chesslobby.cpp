#include "chesslobby.h"

namespace chess
{
    error_e chesslobby::add_player(color_e color, std::string name, int skill, chessplayertype_e ptype)
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        return e_none;
    }

    error_e chesslobby::drop_player(color_e color)
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        return e_none;
    }

    error_e chesslobby::clear_players()
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        return e_none;
    }

    std::shared_ptr<chessplayer> chesslobby::player(color_e col)
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        return NULL;
    }

}