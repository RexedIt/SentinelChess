#include <iostream>
#include <fstream>

#include "chessplayerhub.h"
#include "chessplayer.h"

namespace chess
{

    extern chessplayerhub *p_hub;

    chessplayerdata::chessplayerdata()
    {
        elo = 500;
        ptype = t_none;
        puzzlepoints = 0;
        gamepoints = 0;
        persistent = false;
    }

    chessplayerdata::chessplayerdata(std::string _guid, std::string _username, int32_t _elo, chessplayertype_e _ptype, bool _persistent, std::string _meta)
    {
        guid = _guid;
        username = _username;
        elo = _elo;
        ptype = _ptype;
        persistent = _persistent;
        meta = _meta;
        elo = 0;
        puzzlepoints = 0;
        gamepoints = 0;
    }

    chessplayerdata::chessplayerdata(json j)
    {
        load_json(j);
    }

    error_e chessplayerdata::load_json(json j)
    {
        JSON_LOAD(j, "guid", guid, "");
        JSON_LOAD(j, "username", username, "");
        JSON_LOAD(j, "fullname", fullname, "");
        JSON_LOAD(j, "elo", elo, 0);
        std::string ptypestr;
        JSON_LOAD(j, "ptype", ptypestr, "None");
        ptype = playertypefromstring(ptypestr);
        JSON_LOAD(j, "puzzlepoints", puzzlepoints, 0);
        JSON_LOAD(j, "gamepoints", gamepoints, 0);
        JSON_LOAD(j, "persistent", persistent, true);
        JSON_LOAD(j, "avatar", avatar, "");
        JSON_LOAD(j, "meta", meta, "");
        return e_none;
    }

    error_e chessplayerdata::save_json(json &j, bool full)
    {
        j["guid"] = guid;
        j["username"] = username;
        j["elo"] = elo;
        j["ptype"] = playertypetostring(ptype);
        j["persistent"] = persistent;
        if (full)
        {
            j["fullname"] = fullname;
            j["puzzlepoints"] = puzzlepoints;
            j["gamepoints"] = gamepoints;
            j["avatar"] = avatar;
        }
        j["meta"] = meta;
        return e_none;
    }

    std::string new_guid()
    {
        unsigned char uuid[16];
        for (size_t i = 0; i < 16; i++)
            uuid[i] = (unsigned char)get_rand_int(0, 255);
        char str[37];
        snprintf(str, 37,
                 "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
                 uuid[0], uuid[1], uuid[2], uuid[3], uuid[4], uuid[5], uuid[6], uuid[7],
                 uuid[8], uuid[9], uuid[10], uuid[11], uuid[12], uuid[13], uuid[14], uuid[15]);
        return str;
    }

    chessplayerdata new_computer_player(std::string username, int32_t elo, std::string meta)
    {
        return chessplayerdata(new_guid(), username, elo, t_computer, false, meta);
    }

    chessplayerdata new_human_player(std::string username, int32_t elo, std::string meta)
    {
        return chessplayerdata(new_guid(), username, elo, t_human, false, meta);
    }

    chessplayerdata new_puzzle_player(std::string username, int32_t elo, std::string meta)
    {
        return chessplayerdata(new_guid(), username, elo, t_puzzle, false, meta);
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
        // do not lock this with mutex as it will be called by locked functions
        if (m_filename == "")
            return e_loading;

        try
        {
            json jsonf;

            auto players = json::array();

            for (std::map<std::string, chessplayerdata>::iterator it = m_players.begin(); it != m_players.end(); ++it)
            {
                auto player = json::object();
                if (it->second.persistent)
                    it->second.save_json(player);
            }

            jsonf["Players"] = players;

            std::ofstream os;
            os.open(m_filename, std::ios::binary | std::ios::out);
            os << std::setw(4) << jsonf;
            os.close();
            return e_none;
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << '\n';
            return e_saving;
        }
    }

    void chessplayerhub::clear_players()
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        m_players.clear();
    }

    void chessplayerhub::initialize()
    {
        register_data(new_computer_player("Hank", 500, ""));
        register_data(new_computer_player("Veronica", 1000, ""));
        register_data(new_computer_player("Julian", 1500, ""));
        register_data(new_computer_player("Hal", 2000, ""));
        register_data(new_computer_player("DeepDrew", 2500, ""));
    }

    error_e chessplayerhub::register_data(chessplayerdata data)
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        data.username = trim(data.username);
        if (data.guid == "")
            return e_no_guid;
        m_players[data.guid] = data;
        if (data.persistent)
            save_json();
        return e_none;
    }

    error_e chessplayerhub::get_or_register_player(std::string username, int32_t elo, chessplayertype_e ptype, chessplayerdata &data)
    {
        error_e err = e_none;
        if ((data.guid != "") && (data.username == username))
        {
            err = find_by_id(data.guid, data);
            if (err == e_none)
                return e_none;
        }

        err = find_by_user(username, ptype, data);
        if (err == e_player_not_found)
        {
            data.guid = new_guid();
            data.username = username;
            data.elo = elo;
            data.ptype = ptype;
            err = register_data(data);
        }
        return err;
    }

    error_e chessplayerhub::get_or_register_player(chessplayerdata &data)
    {
        return get_or_register_player(data.username, data.elo, data.ptype, data);
    }

    error_e chessplayerhub::get_matching_computer_player(int32_t elo, chessplayerdata &data)
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        std::vector<std::string> matches;
        for (std::map<std::string, chessplayerdata>::iterator it = m_players.begin(); it != m_players.end(); ++it)
            if (it->second.ptype == t_computer)
                if (abs(it->second.elo - elo) >= 250)
                    matches.push_back(it->second.guid);
        if (matches.size() == 0)
            return e_player_not_found;
        size_t idx = (size_t)get_rand_int(0, (int)(matches.size() - 1));
        data = m_players[matches[idx]];
        return e_none;
    }

    error_e chessplayerhub::unregister(std::string guid)
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        std::map<std::string, chessplayerdata>::iterator it = m_players.find(guid);
        bool persistent = false;
        if (it != m_players.end())
        {
            persistent = it->second.persistent;
            m_players.erase(it);
            if (persistent)
                save_json();
            return e_none;
        }
        return e_player_not_found;
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

    error_e chessplayerhub::find_by_user(std::string username, chessplayertype_e ptype, chessplayerdata &data)
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        std::string uusername = trim(uppercase(username));
        for (std::map<std::string, chessplayerdata>::iterator it = m_players.begin(); it != m_players.end(); ++it)
        {
            if (it->second.ptype == ptype)
                if (uppercase(it->second.username) == uusername)
                {
                    data = it->second;
                    return e_none;
                }
        }
        return e_player_not_found;
    }

}