#define _CRT_SECURE_NO_WARNINGS

#include "chessengine.h"
#include "chessopenings.h"
#include "chessplayerhub.h"

#ifdef _WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#endif

#include <cstdlib>
#include <iostream>

namespace chess
{
    // *** SINGLETON ***
    std::string _data_folder = "..//ChessData/";
    std::string _user_folder = "";
    std::string _test_folder = "./test/TestData/";

    chessecodb *p_eco = NULL;
    chessecodb _co;
    chessplayerhub *p_hub = NULL;
    chessplayerhub _ph;

    error_e chessengine::initialize(int argc, char **argv)
    {
        std::string datapath = "";
        std::string testpath = "";
        for (int i = 1; i < argc; i++)
        {
            std::string arg = argv[i];
            bool has_next = i + 1 < argc;
            if (arg == "--chessdata")
            {
                if (has_next)
                    datapath = fix_path(argv[(i++) + 1]);
            }
            else if (arg == "--testdata")
            {
                if (has_next)
                    testpath = fix_path(argv[(i++) + 1]);
            }
        }
        return initialize(datapath, testpath);
    }

    error_e chessengine::initialize(std::string f, std::string t)
    {
        if (f != "")
        {
            if (!get_dir_exists(f))
            {
                // try an alternative;
                if (get_dir_exists(".\\ChessData\\"))
                {
                    _data_folder = ".\\ChessData\\";
                }
                else
                {
                    return e_data_not_found;
                }
            }
            else
            {
                _data_folder = f;
            }
        }

        if (t != "")
            _test_folder = t;

        error_e err = _co.load_binary(data_file("scid.bin"));
        if (err)
            return err;
        try
        {
            std::string uf = user_folder();
            if (!get_dir_exists(uf))
                if (_mkdir(uf.c_str()) != 0)
                    return e_user_not_found;
            err = _ph.load_players(user_file("playdata.json"));
            if (err == e_none)
                err = _ph.load_computers(data_file("compdata.json"));
            return err;
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << '\n';
            return e_user_not_found;
        }
    }

    std::string chessengine::data_file(std::string f)
    {
        if (_data_folder == "")
            return f;
        char lc = _data_folder[_data_folder.length() - 1];
        if ((lc == '\\') || (lc == '/'))
            return fix_path(_data_folder + f);
        return fix_path(_data_folder + "\\" + f);
    }

    std::string chessengine::data_folder()
    {
        return _data_folder;
    }

    std::string chessengine::user_folder()
    {
#ifdef _WIN32
        if (_user_folder == "")
        {
            _user_folder = std::getenv("APPDATA");
            _user_folder += "\\SentinelChess";
        }
        return _user_folder;
#else
        return "~/.local/share/SentinelChess";
#endif
    }

    std::string chessengine::test_folder()
    {
        return _test_folder;
    }

    std::string chessengine::user_file(std::string f)
    {
        return fix_path(user_folder() + "\\" + f);
    }

    // static chessecodb calls
    error_e chessengine::chessopenings(std::vector<chessopening> &openings)
    {
        if (p_eco)
            return p_eco->chessopenings(openings);
        return e_no_openings;
    }

    // static chessplayerhub calls
    error_e chessengine::hub_register_data(chessplayerdata data)
    {
        if (p_hub)
            return p_hub->register_data(data);
        return e_no_player_hub;
    }

    error_e chessengine::hub_register_player(chessplayerdata data)
    {
        if (p_hub)
            return p_hub->register_player(data);
        return e_no_player_hub;
    }

    error_e chessengine::hub_register_computer(chessplayerdata data)
    {
        if (p_hub)
            return p_hub->register_computer(data);
        return e_no_player_hub;
    }

    error_e chessengine::hub_unregister(std::string guid)
    {
        if (p_hub)
            return p_hub->unregister(guid);
        return e_no_player_hub;
    }

    error_e chessengine::hub_get_or_register_player(chessplayerdata &data, std::string username, int32_t elo, chessplayertype_e ptype)
    {
        if (p_hub)
            return p_hub->get_or_register_player(data, username, elo, ptype);
        return e_no_player_hub;
    }

    error_e chessengine::hub_get_or_register_player(chessplayerdata &data)
    {
        if (p_hub)
            return p_hub->get_or_register_player(data);
        return e_no_player_hub;
    }

    error_e chessengine::hub_get_matching_computer_player(chessplayerdata &data, std::string username, int32_t elo)
    {
        if (p_hub)
            return p_hub->get_matching_computer_player(data, username, elo);
        return e_no_player_hub;
    }

    error_e chessengine::hub_usernames(std::vector<std::string> &vec, chessplayertype_e ptype, int32_t elo)
    {
        if (p_hub)
        {
            vec = p_hub->usernames(ptype, elo);
            return e_none;
        }
        return e_no_player_hub;
    }

    error_e chessengine::hub_players(std::vector<chessplayerdata> &vec, chessplayertype_e ptype, bool include_avatars, int32_t elo, bool sort_elo)
    {
        if (p_hub)
        {
            vec = p_hub->players(ptype, include_avatars, elo, sort_elo);
            return e_none;
        }
        return e_no_player_hub;
    }

    error_e chessengine::hub_refresh_player(chessplayerdata &data)
    {
        if (p_hub)
            return p_hub->refresh_player(data);
        return e_no_player_hub;
    }

    error_e chessengine::hub_update_player(chessplayerdata data)
    {
        if (p_hub)
            return p_hub->update_player(data);
        return e_no_player_hub;
    }

    error_e chessengine::hub_update_points(std::string guid, int32_t pts, std::string puzzle)
    {
        if (p_hub)
            return p_hub->update_points(guid, pts, puzzle);
        return e_no_player_hub;
    }

}

#undef _CRT_SECURE_NO_WARNINGS