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

    error_e chesslobby::new_game()
    {
        attach_to_game();
        return mp_game->new_game();
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

    error_e chesslobby::new_game(color_e user_color, std::string name, int skill, chessplayertype_e ptype)
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

        err = mp_game->new_game();
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

            std::string header = load_string(is);
            if (header != "SENTINEL_CHESS")
            {
                is.close();
                return restore(e_loading);
            }

            mp_game = std::shared_ptr<chessgame>(new chessgame());
            attach_to_game();

            error_e err = load_players(is);
            if (err != e_none)
            {
                is.close();
                return restore(err);
            }

            err = mp_game->load_game(is);
            is.close();

            if (err != e_none)
                return restore(err);

            attach_to_game();
            return err;
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
            std::ofstream os;
            os.open(filename, std::ios::binary | std::ios::out);

            save_string("SENTINEL_CHESS", os);

            error_e err = save_players(os);
            if (err != e_none)
            {
                os.close();
                return err;
            }

            err = mp_game->save_game(os);
            os.close();
            return err;
        }
        catch (const std::exception &)
        {
            return e_saving;
        }
    }

    error_e chesslobby::add_player(color_e color, std::string name, int skill, chessplayertype_e ptype)
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        switch (ptype)
        {
        case t_human:
        {
            std::shared_ptr<chessplayer> p_chessplayer(new chessplayer(color, name, skill, t_human));
            mp_players[color] = p_chessplayer;
            return e_none;
        }
        case t_computer:
        {
            std::shared_ptr<chesscomputer> p_chesscomputer(new chesscomputer(color, name, skill));
            // if the type is a listener, do this
            mp_game->listen(p_chesscomputer);
            mp_players[color] = p_chesscomputer;
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
        mp_players.clear();
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

    error_e chesslobby::load_players(std::ifstream &is)
    {
        try
        {
            int8_t n = 0;
            is.read((char *)&n, sizeof(n));
            mp_players.clear();
            for (int8_t i = 0; i < n; i++)
            {
                color_e color;
                is.read((char *)&color, sizeof(color));
                std::string name = load_string(is);
                int32_t skill;
                is.read((char *)&skill, sizeof(skill));
                chessplayertype_e ptype = t_none;
                is.read((char *)ptype, sizeof(ptype));
                if (add_player(color, name, skill, ptype) != e_none)
                    return e_loading;
            }
            return e_none;
        }
        catch (const std::exception &)
        {
            return e_loading;
        }
    }

    error_e chesslobby::save_players(std::ofstream &os)
    {
        try
        {
            int8_t n = (int8_t)mp_players.size();
            os.write((char *)&n, sizeof(n));
            for (const auto &kv : mp_players)
            {
                color_e color = kv.second->playercolor();
                os.write((char *)&color, sizeof(color));
                save_string(kv.second->playername(), os);
                int32_t skill = kv.second->playerskill();
                os.write((char *)&skill, sizeof(skill));
                chessplayertype_e t = kv.second->playertype();
                os.write((char *)&t, sizeof(t));
            }
            return e_none;
        }
        catch (const std::exception &)
        {
            return e_saving;
        }
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