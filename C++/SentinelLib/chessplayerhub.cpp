#include <iostream>
#include <fstream>
#include <sstream>

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
        puzzlepoints = 0;
        gamepoints = 0;
    }

    chessplayerdata::chessplayerdata(json j, chessplayertype_e _ptype)
    {
        load_json(j);
        if (_ptype != t_none)
            ptype = _ptype;
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

        if (j.contains("meta"))
        {
            auto mj = j["meta"];
            meta = mj.dump();
        }

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
        if (meta != "")
        {
            auto mj = json::object();
            std::istringstream ifs(meta);
            ifs >> mj;
            j["meta"] = mj;
        }
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

    chessplayerdata new_computer_player(std::string guid, std::string username, int32_t elo, std::string meta)
    {
        return chessplayerdata(guid, username, elo, t_computer, false, meta);
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
    }

    chessplayerhub::~chessplayerhub()
    {
        save_json();
        m_humans.clear();
        p_hub = NULL;
    }

    error_e chessplayerhub::load_players(std::string filename)
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
                register_data(chessplayerdata(player, t_human));

            return e_none;
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << '\n';
            return e_loading;
        }
    }

    error_e chessplayerhub::load_computers(std::string filename)
    {
        try
        {
            std::ifstream is;
            is.open(filename, std::ios::binary | std::ios::in);
            if (!is.is_open())
                return e_loading;
            json jsonf;
            is >> jsonf;
            is.close();

            auto players = jsonf["Players"];
            m_computers.clear();

            for (auto player : players)
                register_data(chessplayerdata(player, t_computer));

            return e_none;
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << '\n';
            return e_loading;
        }
    }

    error_e chessplayerhub::save_json(bool persistent)
    {
        // do not lock this with mutex as it will be called by locked functions
        if (m_filename == "")
            return e_loading;

        try
        {
            json jsonf;

            auto players = json::array();

            for (std::map<std::string, chessplayerdata>::iterator it = m_humans.begin(); it != m_humans.end(); ++it)
            {
                auto player = json::object();
                if (it->second.persistent == persistent)
                    it->second.save_json(player);
                players.push_back(player);
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
        m_humans.clear();
    }

    error_e chessplayerhub::register_data(chessplayerdata data)
    {
        if (data.ptype == t_human)
            return register_player(data);
        else if (data.ptype == t_computer)
            return register_computer(data);
        else
            return e_cannot_register;
    }

    error_e chessplayerhub::register_player(chessplayerdata data)
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        data.username = trim(data.username);
        if (data.guid == "")
            return e_no_guid;
        if (data.username == "")
            return e_no_username;
        if (data.ptype != t_human)
            return e_cannot_register;
        if (m_humans.count(data.guid))
            return e_player_already_registered;
        for (auto kv : m_humans)
            if (uppercase(kv.second.username) == uppercase(data.username))
                return e_player_already_registered;
        m_humans[data.guid] = data;
        if (data.persistent)
            save_json();
        return e_none;
    }

    error_e chessplayerhub::register_computer(chessplayerdata data)
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        data.username = trim(data.username);
        if (data.guid == "")
            return e_no_guid;
        if (data.username == "")
            return e_no_username;
        if (data.ptype != t_computer)
            return e_cannot_register;
        if (m_computers.count(data.guid))
            return e_player_already_registered;
        for (auto kv : m_computers)
            if (uppercase(kv.second.username) == uppercase(data.username))
                return e_player_already_registered;
        m_computers[data.guid] = data;
        return e_none;
    }

    error_e chessplayerhub::get_or_register_player(chessplayerdata &data, std::string username, int32_t elo, chessplayertype_e ptype)
    {
        error_e err = e_none;
        if (ptype == t_computer)
            return get_matching_computer_player(data, username, elo);
        // Humans
        if ((data.guid != "") && (data.username == username))
        {
            err = find_by_id(data, data.guid);
            if (err == e_none)
                return e_none;
        }
        err = find_player_by_username(data, username, ptype);
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
        return get_or_register_player(data, data.username, data.elo, data.ptype);
    }

    error_e chessplayerhub::get_matching_computer_player(chessplayerdata &data, std::string username, int32_t elo)
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        std::vector<std::string> matches;
        std::string uusername = uppercase(username);
        for (std::map<std::string, chessplayerdata>::iterator it = m_computers.begin(); it != m_computers.end(); ++it)
        {
            if ((uusername != "") && (uusername == uppercase(it->second.username)))
            {
                data = it->second;
                return e_none;
            }
            if (abs(it->second.elo - elo) <= 250)
                matches.push_back(it->second.guid);
        }
        if (matches.size() == 0)
            return e_player_not_found;
        size_t idx = (size_t)get_rand_int(0, (int)(matches.size() - 1));
        data = m_computers[matches[idx]];
        return e_none;
    }

    error_e chessplayerhub::unregister(std::string guid)
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        std::map<std::string, chessplayerdata>::iterator it = m_humans.find(guid);
        bool persistent = false;
        if (it != m_humans.end())
        {
            persistent = it->second.persistent;
            m_humans.erase(it);
            if (persistent)
                save_json();
            return e_none;
        }
        return e_player_not_found;
    }

    error_e chessplayerhub::find_by_id(chessplayerdata &data, std::string guid)
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        if (m_humans.count(guid))
        {
            data = m_humans[guid];
            return e_none;
        }
        if (m_computers.count(guid))
        {
            data = m_computers[guid];
            return e_none;
        }
        return e_player_not_found;
    }

    error_e chessplayerhub::find_player_by_username(chessplayerdata &data, std::string username, chessplayertype_e ptype)
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        std::string uusername = trim(uppercase(username));
        for (std::map<std::string, chessplayerdata>::iterator it = m_humans.begin(); it != m_humans.end(); ++it)
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

    std::vector<std::string> chessplayerhub::usernames(chessplayertype_e ptype, int32_t elo)
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        std::vector<std::string> ret;
        if (ptype == t_human)
        {
            for (std::map<std::string, chessplayerdata>::iterator it = m_humans.begin(); it != m_humans.end(); ++it)
            {
                bool match = true;
                if (elo > 0)
                {
                    int32_t celo = it->second.elo;
                    match = ((celo >= elo - 250) && (celo <= elo + 250));
                }
                if (match)
                    ret.push_back(it->second.username);
            }
        }
        else if (ptype == t_computer)
        {
            for (std::map<std::string, chessplayerdata>::iterator it = m_computers.begin(); it != m_computers.end(); ++it)
            {
                bool match = true;
                if (elo > 0)
                {
                    int32_t celo = it->second.elo;
                    match = ((celo >= elo - 250) && (celo <= elo + 250));
                }
                if (match)
                    ret.push_back(it->second.username);
            }
        }
        std::sort(ret.begin(), ret.end());
        return ret;
    }

    std::vector<chessplayerdata> chessplayerhub::players(chessplayertype_e ptype, bool include_avatars, int32_t elo, bool sort_elo)
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        std::vector<chessplayerdata> ret;
        if (ptype == t_human)
        {
            for (std::map<std::string, chessplayerdata>::iterator it = m_humans.begin(); it != m_humans.end(); ++it)
            {
                chessplayerdata pd = it->second;
                bool match = true;
                if (elo > 0)
                {
                    int32_t celo = pd.elo;
                    match = ((celo >= elo - 250) && (celo <= elo + 250));
                }
                if (match)
                {
                    if (include_avatars == false)
                        pd.avatar = "";
                    ret.push_back(pd);
                }
            }
        }
        else if (ptype == t_computer)
        {
            for (std::map<std::string, chessplayerdata>::iterator it = m_computers.begin(); it != m_computers.end(); ++it)
            {
                chessplayerdata pd = it->second;
                bool match = true;
                if (elo > 0)
                {
                    int32_t celo = pd.elo;
                    match = ((celo >= elo - 250) && (celo <= elo + 250));
                }
                if (match)
                {
                    if (include_avatars == false)
                        pd.avatar = "";
                    ret.push_back(pd);
                }
            }
        }
        if (sort_elo)
        {
            sort(ret.begin(), ret.end(),
                 [](const chessplayerdata &a, const chessplayerdata &b)
                 {
                     return a.elo < b.elo;
                 });
        }
        else
        {
            sort(ret.begin(), ret.end(),
                 [](const chessplayerdata &a, const chessplayerdata &b)
                 {
                     return a.username < b.username;
                 });
        }
        return ret;
    }

}