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
        generate_static_players();
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
        std::map<std::string, chessplayerdata> static_players;
        for (std::map<std::string, chessplayerdata>::iterator it = m_players.begin(); it != m_players.end(); ++it)
        {
            chessplayerdata d = it->second;
            if ((d.persistent == false) && (d.ptype == t_computer))
                static_players[d.guid] = d;
        }
        m_players = static_players;
    }

    void chessplayerhub::generate_static_players()
    {
        // Template for computer metadata: {"bp_weight":0.5,"eco_favorites":["ABC","DEF"],"eco_weight":32,"kc_weight":0.25,"turn_time":5000}
        register_data(new_computer_player("87f7663e-95dc-430d-95e2-240ff4e4f285", "Sparky", 400, R"({"bp_weight":0.0,"eco_weight":0,"kc_weight":0.0})"));
        register_data(new_computer_player("44e57420-4b35-4c90-abb7-ac6d13081f9d", "Hank", 500, R"({"bp_weight":0.0,"eco_weight":8,"kc_weight":0.0})"));
        register_data(new_computer_player("83b4cab2-f465-4f86-b73b-e03276d2387f", "Corky", 650, R"({"eco_weight":16,"eco_favorites":["A01", D06"]})"));
        register_data(new_computer_player("bc28442a-c928-4cc6-9315-e157952953cc", "Zippy", 850, R"({"eco_weight":32,"eco_favorites":["D06","A04"],"turn_time":2000})"));
        register_data(new_computer_player("49379cc9-15e9-4934-95a0-4b0a47dbd414", "Veronica", 1000, R"({"eco_weight":32,"eco_favorites":["D06","C44","C45"]})"));
        register_data(new_computer_player("496c83f0-59be-4c0e-a4ca-f8bd098722a2", "Vlad", 1150, R"({"eco_weight":32,"eco_favorites":["B20","B25","B39"],"kc_weight":0.65,"bp_weight":1.0})"));
        register_data(new_computer_player("dd2c6c0e-1e60-4e1f-8971-2042fbec714c", "Benjamin", 1350, R"({"eco_weight":16,"eco_favorites":["D08","E54"],"kc_weight":0.25,"bp_weight":0.25})"));
        register_data(new_computer_player("40d6077c-b107-4271-bbb0-9b21b259a8fc", "Julian", 1500, R"({"bp_weight":0.75,"eco_favorites":["A00","A04","C25","C26","C27","C28"],"eco_weight":64,"kc_weight":0.5,"turn_time":7500})"));
        register_data(new_computer_player("d485b7ff-e883-4680-b092-8329478f2e18", "Dorian", 1750, R"({"bp_weight":0.4,"eco_favorites":["A00","A04","D06","D07","D09","D10"],"eco_weight":64,"kc_weight":0.15,"turn_time":9000})"));
        register_data(new_computer_player("f9b73070-17d1-403a-983f-eed8eba9e283", "Hal", 2000, R"({"bp_weight":0.25,"eco_favorites":["C21","C22","C60","B00"],"eco_weight":64,"kc_weight":0.15,"turn_time":10000})"));
        register_data(new_computer_player("5f20e613-4d6c-46a6-84ec-452b436ff637", "DeepDrew", 2500, R"({"bp_weight":0.25,"eco_favorites":["D00","C21","C22","C23","C25","C26","C27","C28","D06","D07","D08","D09","C60","C44","C45","A10","A04"],"eco_weight":64,"kc_weight":0.15,"turn_time":10000})"));
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