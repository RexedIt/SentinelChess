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
    std::string _data_folder = "..\\ChessData\\";
    std::string _user_folder = "";

    chessecodb *p_eco = NULL;
    chessecodb _co;
    chessplayerhub *p_hub = NULL;
    chessplayerhub _ph;

    error_e chessengine::initialize(std::string f)
    {
        if (!get_dir_exists(f))
            return e_data_not_found;
        _data_folder = f;

        error_e err = _co.load_binary(data_file("scid.bin"));
        if (err)
            return err;
        try
        {
            std::string uf = user_folder();
            if (!get_dir_exists(uf))
                if (_mkdir(uf.c_str()) != 0)
                    return e_user_not_found;
            err = _ph.load_json(user_file("playdata.json"));
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
            _user_folder = getenv("APPDATA");
            _user_folder += "\\SentinelChess";
        }
        return _user_folder;
#else
        return "~/.local/share/SentinelChess";
#endif
    }

    std::string chessengine::user_file(std::string f)
    {
        return fix_path(user_folder() + "\\" + f);
    }

    error_e chessengine::chessopenings(std::vector<chessopening> &openings)
    {
        if (p_eco)
            return p_eco->chessopenings(openings);
        return e_no_openings;
    }

    error_e chessengine::preferredecos(color_e col, std::vector<std::string> &ecos)
    {
        if (p_eco)
            return p_eco->preferredecos(col, ecos);
        return e_no_openings;
    }
}