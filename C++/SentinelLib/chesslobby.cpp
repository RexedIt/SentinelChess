#include "chessengine.h"
#include "chesslobby.h"
#include "chessplayer.h"
#include "chesscomputer.h"
#include "chesspuzzleplayer.h"
#include "chesspgn.h"

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

    error_e chesslobby::new_game(std::string title, const chessclock_s &clock)
    {
        attach_to_game();
        write_player_meta();
        return mp_game->new_game(title, clock);
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

    error_e chesslobby::new_game(std::string title, color_e user_color, std::string user_name, int elo, const chessclock_s &clock)
    {
        // intended for a quick, temporary game
        backup();

        clear_players();

        chessplayerdata pd;
        error_e err = chessengine::hub_get_or_register_player(pd, user_name, elo, t_human);
        if (err != e_none)
            return restore(err);

        err = add_player(user_color, pd);
        if (err != e_none)
            return restore(err);

        // create game object early
        mp_game = std::shared_ptr<chessgame>(new chessgame());

        // Create a computer matchup
        chessplayerdata cd;
        err = chessengine::hub_get_matching_computer_player(cd, "", elo);
        if (err != e_none)
            return restore(err);

        err = add_player(other(user_color), cd);
        if (err != e_none)
            return restore(err);

        err = mp_game->new_game(title, clock);
        if (err != e_none)
            return restore(err);

        attach_to_game();
        return err;
    }

    error_e chesslobby::load_game(std::string filename, std::string &errextra)
    {
        bool pgn = ends_with(uppercase(filename), ".PGN");
        return pgn ? load_pgn(filename, errextra) : load_chs(filename, errextra);
    }

    error_e chesslobby::save_game(std::string filename)
    {
        bool pgn = ends_with(uppercase(filename), ".PGN");
        return pgn ? save_pgn(filename) : save_chs(filename);
    }

    error_e chesslobby::load_chs(std::string filename, std::string &errextra)
    {
        errextra = "";
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

            error_e err = e_none;

            for (auto it : players)
            {
                color_e color = str_color(it["color"]);
                chessplayerdata pd;
                pd.load_json(it);
                err = chessengine::hub_get_or_register_player(pd);
                if (err != e_none)
                    return restore(err);
                if (add_player(color, pd) != e_none)
                    return restore(e_loading);
            }

            write_player_meta();
            err = mp_game->load_chs(jsonf);
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

    error_e chesslobby::save_chs(std::string filename)
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
                kv.second->playerdata().save_json(player, false);
                players.push_back(player);
            }

            jsonf["Players"] = players;

            error_e err = mp_game->save_chs(jsonf);
            if (err == e_none)
            {
                std::ofstream os;
                os.open(filename, std::ios::binary | std::ios::out);
                os << std::setw(4) << jsonf;
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

    error_e chesslobby::load_puzzle(chessplayerdata user, chesspuzzle p)
    {
        // determine the human color
        chessboard b;
        error_e err = b.load_xfen(p.fen);
        if (err != e_none)
            return err;
        color_e tc = b.turn_color();

        backup();
        // create game object early
        mp_game = std::shared_ptr<chessgame>(new chessgame());
        attach_to_game();

        clear_players();

        err = add_player(other(tc), user);
        if (err != e_none)
            return restore(err);

        err = add_player(tc, new_puzzle_player("Puzzle", p.rating));
        if (err != e_none)
            return restore(err);

        write_player_meta(true);
        err = mp_game->load_puzzle(p);
        if (err != e_none)
            return restore(err);

        attach_to_game();
        return err;
    }

    error_e chesslobby::load_puzzle(chessplayerdata user, std::string filename, std::string keywords, int rating)
    {
        // determine the human color
        chesspuzzle p;
        // specific?
        error_e err = e_none;
        if ((keywords.length()) && (keywords[0] == '!'))
            err = p.load_specific(filename, keywords.substr(1));
        else
            err = p.load_random(filename, keywords, rating, user.visited);
        if (err != e_none)
            return err;
        return load_puzzle(user, p);
    }

    error_e chesslobby::load_puzzle(chessplayerdata user, std::string contents)
    {
        chesspuzzle p;
        error_e err = p.load_line(contents);
        if (err != e_none)
            return err;
        return load_puzzle(user, p);
    }

    error_e chesslobby::save_pgn(std::string filename)
    {
        try
        {
            chesspgn p;

            for (const auto &kv : mp_players)
            {
                std::string colname = color_str(kv.second->playercolor());
                p.write_tag(colname, kv.second->playername());
                p.write_tag(colname + "Elo", kv.second->playerskill());
            }

            error_e err = mp_game->save_pgn(p);
            if (err == e_none)
                err = p.save(filename);
            return err;
        }
        catch (const std::exception &)
        {
            return e_saving;
        }
        return e_none;
    }

    error_e chesslobby::load_pgn(std::string filename, std::string &errextra)
    {
        // Load the pgn first
        errextra = "";
        chesspgn p;
        error_e err = p.load(filename, errextra);
        if (err != e_none)
            return err;

        backup();

        // create game object early
        mp_game = std::shared_ptr<chessgame>(new chessgame());
        attach_to_game();

        clear_players();

        chessplayerdata pw = p.get_playerdata(c_white);
        err = chessengine::hub_get_or_register_player(pw);
        if (err != e_none)
            return restore(err);

        chessplayerdata pb = p.get_playerdata(c_black);
        err = chessengine::hub_get_or_register_player(pb);
        if (err != e_none)
            return restore(err);

        err = add_player(c_white, pw);
        if (err != e_none)
            return restore(err);

        err = add_player(c_black, pb);
        if (err != e_none)
            return restore(err);

        write_player_meta();
        err = mp_game->load_pgn(p);
        if (err != e_none)
            return restore(err);

        attach_to_game();
        return err;
    }

    error_e chesslobby::add_player(color_e color, chessplayerdata data)
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        switch (data.ptype)
        {
        case t_human:
        {
            std::shared_ptr<chessplayer> p(new chessplayer(color, data));
            m_locals.insert(color);
            mp_players[color] = p;
            p->set_game(mp_game);
            return e_none;
        }
        case t_computer:
        {
            std::shared_ptr<chesscomputer> p(new chesscomputer(color, data));
            // if the type is a listener, do this
            mp_game->listen(p);
            mp_players[color] = p;
            p->set_game(mp_game);
            return e_none;
        }
        case t_puzzle:
        {
            std::shared_ptr<chesspuzzleplayer> p(new chesspuzzleplayer(color, data));
            // if the type is a listener, do this
            mp_game->listen(p);
            mp_players[color] = p;
            p->set_game(mp_game);
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
                kv.second->stop_listening();

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

    void chesslobby::potential_points(color_e col, int32_t &win, int32_t &lose, int32_t &draw)
    {
        if (mp_game)
            return mp_game->potential_points(col, win, lose, draw);
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

    void chesslobby::write_player_meta(bool puzzle)
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        for (const auto &kv : mp_players)
        {
            std::shared_ptr<chessplayer> player = kv.second;
            chessplayertype_e ptype = player->playertype();
            if (ptype != t_puzzle)
            {
                color_e pcolor = player->playercolor();
                int32_t pelo = puzzle ? player->playerpuzzlepoints() : player->playerskill();
                std::string pguid = player->playerguid();
                std::string colname = color_str(pcolor);
                mp_game->write_tag(colname + "Elo", pelo);
                mp_game->write_tag(colname + "Guid", pguid);
                mp_game->write_tag(colname + "Type", playertypetostring(ptype));
            }
        }
    }

}