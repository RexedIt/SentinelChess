#include "chesslobby.h"
#include "chessplayer.h"
#include "chesscomputer.h"

namespace chess
{

    chesslobby::chesslobby(std::shared_ptr<chessgamelistener> p_listener)
    {
        mp_game = std::shared_ptr<chessgame>(new chessgame());
        set_listener(p_listener);
    }

    void chesslobby::set_listener(std::shared_ptr<chessgamelistener> p_listener)
    {
        if (p_listener)
            mp_game->listen(p_listener);
        else
        {
            if (mp_listener)
                mp_game->unlisten(mp_listener->id());
        }
        mp_listener = p_listener;
    }

    chesslobby::~chesslobby()
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        mp_game = NULL;
        mp_game_backup = NULL;
        mp_players.clear();
        mp_players_backup.clear();
    }

    error_e chesslobby::new_game(const chessclock_s &clock)
    {
        attach_to_game();
        return mp_game->new_game(clock);
    }

    void chesslobby::attach_to_game()
    {
        if (mp_listener)
            mp_game->listen(mp_listener);
        for (const auto &kv : mp_players)
            kv.second->set_game(mp_game);
    }

    void chesslobby::detach_from_game()
    {
        for (const auto &kv : mp_players)
            kv.second->set_game(NULL);
    }

    error_e chesslobby::new_game(color_e user_color, std::string name, int skill, chessplayertype_e ptype, const chessclock_s &clock)
    {
        backup();

        clear_players();
        error_e err = add_player(user_color, name, skill, ptype);
        if (err != e_none)
            return restore(err);

        // create game object early
        mp_game = std::shared_ptr<chessgame>(new chessgame());

        // Create a computer matchup
        err = add_player(other(user_color), "Computer", skill, t_computer);
        if (err != e_none)
            return restore(err);

        err = mp_game->new_game(clock);
        if (err != e_none)
            return restore(err);

        attach_to_game();
        return err;
    }

    error_e chesslobby::load_game(std::string filename)
    {
        backup();
        try
        {
            std::ifstream is;
            is.open(filename, std::ios::binary | std::ios::in);
            if (!is.is_open())
                return restore(e_loading);

            json jsonf;
            is >> jsonf;
            is.close();

            if (jsonf["Header"] != "SENTINEL_CHESS")
                return restore(e_loading);

            mp_game = std::shared_ptr<chessgame>(new chessgame());
            attach_to_game();

            auto players = jsonf["Players"];
            clear_players();

            for (auto it : players)
            {
                color_e color = str_color(it["color"]);
                std::string name = it["name"];
                int32_t skill = it["skill"];
                chessplayertype_e ptype = it["type"];
                if (add_player(color, name, skill, ptype) != e_none)
                    return e_loading;
            }

            error_e err = mp_game->load_game(jsonf);
            if (err != e_none)
                return restore(err);

            attach_to_game();
            return e_none;
        }
        catch (const std::exception &)
        {
            return restore(e_loading);
        }
    }

    error_e chesslobby::save_game(std::string filename)
    {
        try
        {
            json jsonf;

            jsonf["Header"] = "SENTINEL_CHESS";

            auto players = json::array();

            for (const auto &kv : mp_players)
            {
                auto player = json::object();
                player["color"] = color_str(kv.second->playercolor());
                player["name"] = kv.second->playername();
                player["skill"] = kv.second->playerskill();
                player["type"] = kv.second->playertype();
                players.push_back(player);
            }

            jsonf["Players"] = players;

            error_e err = mp_game->save_game(jsonf);
            if (err == e_none)
            {
                std::ofstream os;
                os.open(filename, std::ios::binary | std::ios::out);
                os << jsonf;
                os.close();
            }
            return err;
        }
        catch (const std::exception &)
        {
            return e_saving;
        }
        return e_none;
    }

    error_e chesslobby::add_player(color_e color, std::string name, int skill, chessplayertype_e ptype)
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        switch (ptype)
        {
        case t_human:
        {
            std::shared_ptr<chessplayer> p_chessplayer(new chessplayer(color, name, skill, t_human));
            m_locals.insert(color);
            mp_players[color] = p_chessplayer;
            p_chessplayer->set_game(mp_game);
            return e_none;
        }
        case t_computer:
        {
            std::shared_ptr<chesscomputer> p_chesscomputer(new chesscomputer(color, name, skill));
            // if the type is a listener, do this
            mp_game->listen(p_chesscomputer);
            mp_players[color] = p_chesscomputer;
            p_chesscomputer->set_game(mp_game);
            return e_none;
        }
        }
        return e_player_not_created;
    }

    error_e chesslobby::drop_player(color_e color)
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        if (mp_players.count(color))
        {
            mp_players.erase(color);
            return e_player_not_found;
        }
        return e_none;
    }

    error_e chesslobby::clear_players()
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        if (mp_game)
            for (const auto &kv : mp_players)
            {
                std::shared_ptr<chesscomputer> pc = std::dynamic_pointer_cast<chesscomputer>(kv.second);
                if (pc)
                    mp_game->unlisten(pc->id());
            }

        mp_players.clear();
        m_locals.clear();
        return e_none;
    }

    std::shared_ptr<chessgame> chesslobby::game()
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        return mp_game;
    }

    std::shared_ptr<chessplayer> chesslobby::player(color_e col)
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        if (mp_players.count(col))
            return mp_players[col];
        return NULL;
    }

    std::map<color_e, std::shared_ptr<chessplayer>> chesslobby::players()
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        return mp_players;
    }

    bool chesslobby::has_local()
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        return m_locals.size() > 0;
    }

    std::set<color_e> chesslobby::local_players()
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        return m_locals;
    }

    bool chesslobby::is_local(color_e col)
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        return m_locals.count(col) > 0;
    }

    bool chesslobby::is_local_turn()
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        if (mp_game)
            return m_locals.count(mp_game->turn_color()) > 0;
        return false;
    }

    bool chesslobby::is_local_active(color_e col)
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        if (mp_game)
            return ((m_locals.count(col) > 0) && (mp_game->turn_color() == col));
        return false;
    }

    std::string chesslobby::player_name(color_e col)
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        if (mp_players.count(col))
            return mp_players[col]->playername();
        return "";
    }

    std::map<color_e, std::string> chesslobby::player_names()
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        std::map<color_e, std::string> ret;
        for (const auto &kv : mp_players)
            ret[kv.second->playercolor()] = kv.second->playername();
        return ret;
    }

    // functions to restore values
    void chesslobby::backup()
    {
        detach_from_game();
        mp_game_backup = mp_game;
        mp_players_backup = mp_players;
    }

    error_e chesslobby::restore(error_e err)
    {
        mp_game = mp_game_backup;
        mp_players_backup = mp_players;
        mp_game_backup = NULL;
        mp_players_backup.clear();
        attach_to_game();
        return err;
    }

}