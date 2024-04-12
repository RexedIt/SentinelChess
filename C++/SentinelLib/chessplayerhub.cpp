#include <iostream>
#include <fstream>

#include "chessplayerhub.h"
#include "chessplayer.h"

namespace chess
{

    extern chessplayerhub *p_hub;

    chessplayerdata::chessplayerdata()
    {
        elo = 0;
        playertype = t_none;
        puzzlepoints = 0;
        gamepoints = 0;
        persistent = false;
    }

    chessplayerdata::chessplayerdata(std::string _guid, std::string _username, int32_t _elo, chessplayertype_e _playertype, bool _persistent, std::string _meta)
    {
        guid = _guid;
        username = _username;
        elo = _elo;
        playertype = _playertype;
        persistent = _persistent;
        meta = _meta;
        elo = 0;
        puzzlepoints = 0;
        gamepoints = 0;
    }

    chessplayerdata::chessplayerdata(json j)
    {
        JSON_LOAD(j, "guid", guid, "");
        JSON_LOAD(j, "username", username, "");
        JSON_LOAD(j, "elo", elo, 0);
        std::string ptype;
        JSON_LOAD(j, "playertype", ptype, "None");
        playertype = playertypefromstring(ptype);
        JSON_LOAD(j, "puzzlepoints", puzzlepoints, 0);
        JSON_LOAD(j, "gamepoints", gamepoints, 0);
        JSON_LOAD(j, "persistent", persistent, true);
        JSON_LOAD(j, "avatar", avatar, "");
        JSON_LOAD(j, "meta", meta, "");
    }

    error_e chessplayerdata::save_json(json &j)
    {
        j["guid"] = guid;
        j["username"] = username;
        j["elo"] = elo;
        j["playertype"] = playertypetostring(playertype);
        j["puzzlepoints"] = puzzlepoints;
        j["gamepoints"] = gamepoints;
        j["persistent"] = persistent;
        j["avatar"] = avatar;
        j["meta"] = meta;
        return e_none;
    }

    chessplayerhub::chessplayerhub()
    {
        if (p_hub != NULL)
        {
            std::cerr << " *** SINGLETON ERROR *** - chessplayerhub already running!" << std::endl;
        }
        p_hub = this;
        initialize();
    }

    chessplayerhub::~chessplayerhub()
    {
        save_json();
        m_players.clear();
        p_hub = NULL;
    }

    error_e chessplayerhub::load_json(std::string filename)
    {
        m_filename = filename;
        try
        {
            std::ifstream is;
            is.open(filename, std::ios::binary | std::ios::in);
            if (!is.is_open())
                return e_none; // can be OK on first time
            json jsonf;
            is >> jsonf;
            is.close();

            auto players = jsonf["Players"];
            clear_players();

            for (auto player : players)
                register_data(chessplayerdata(player));

            return e_none;
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << '\n';
            return e_loading;
        }
    }

    error_e chessplayerhub::save_json()
    {
        if (m_filename == "")
            return e_loading;
        return e_none;
    }

    void chessplayerhub::clear_players()
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        m_players.clear();
    }

    void chessplayerhub::initialize()
    {
        register_data(chessplayerdata("82ce35e3-0fb0-47e6-a1ef-b00fdd8653ec", "Hank", 500, t_computer, false, ""));
        register_data(chessplayerdata("9411dee9-831f-44e3-b834-1e535e5fd7b5", "Veronica", 1000, t_computer, false, ""));
        register_data(chessplayerdata("52dc478c-fadb-4d4d-8ef6-9396762850ae", "Julian", 1500, t_computer, false, ""));
        register_data(chessplayerdata("644762af-0b50-4d13-a300-c8a7ccc4db76", "Hal", 2000, t_computer, false, ""));
    }

    error_e chessplayerhub::register_data(chessplayerdata data)
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        m_players[data.guid] = data;
        return e_none;
    }

    error_e chessplayerhub::find_by_id(std::string guid, chessplayerdata &data)
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        if (m_players.count(guid))
        {
            data = m_players[guid];
            return e_none;
        }
        return e_player_not_found;
    }

    error_e chessplayerhub::find_by_name(std::string username, chessplayerdata &data)
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        for (std::map<std::string, chessplayerdata>::iterator it = m_players.begin(); it != m_players.end(); ++it)
        {
            if (it->second.username == username)
            {
                data = it->second;
                return e_none;
            }
        }
        return e_player_not_found;
    }

}