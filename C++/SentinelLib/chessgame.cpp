#include "chessgame.h"
#include "chesscomputer.h"

namespace chess
{

    chessgame::chessgame()
    {
        m_state = play_e;
        m_white_type = t_none;
        m_black_type = t_none;
        m_win_color = c_none;
    }

    chessboard chessgame::board()
    {
        chessboard b(m_board);
        return b;
    }

    game_state_e chessgame::state()
    {
        return m_state;
    }

    chessplayertype_e chessgame::white_type()
    {
        return m_white_type;
    }

    chessplayertype_e chessgame::black_type()
    {
        return m_black_type;
    }

    color_e chessgame::turn_color()
    {
        return m_board.turn_color();
    }

    color_e chessgame::win_color()
    {
        return m_win_color;
    }

    chessturn_s chessgame::last_turn()
    {
        if (m_turn.size() == 0)
        {
            chessturn_s m;
            return m;
        }
        return m_turn[m_turn.size() - 1];
    }

    int chessgame::turnno()
    {
        return (int)m_turn.size();
    }

    game_state_e chessgame::is_game_over(color_e col, move_s &m)
    {
        if (m_state == play_e)
        {
            color_e winner_col = col == c_white ? c_black : c_white;
            if (m.mate)
            {
                if (m.check)
                    m_state = checkmate_e;
                else
                    m_state = stalemate_e;
            }
        }
        return m_state;
    }

    error_e chessgame::forfeit(color_e col)
    {
        m_state = forfeit_e;
        m_win_color = other(col);
        return e_none;
    }

    error_e chessgame::move(color_e col, move_s m0)
    {
        if (m_state != play_e)
            return e_invalid_game_state;
        if (col == m_board.turn_color())
        {
            move_s m = m_board.attempt_move(col, m0);
            if (!m.is_valid())
                return m.error;
            m_state = is_game_over(col, m);
            m_turn.push_back(new_turn(m_board, m, col));
            signal_on_move(m, col);
            signal_on_turn();
            if (m_state != play_e)
                return end_game(m_state, m_win_color);
        }
        return e_none;
    }

    error_e chessgame::move(color_e col, coord_s p0, coord_s p1, piece_e promote)
    {
        move_s m(p0, p1, promote);
        return move(col, m);
    }

    std::vector<move_s> chessgame::possible_moves(color_e col)
    {
        return m_board.possible_moves(col);
    }

    bool chessgame::check_state(color_e col)
    {
        return m_board.check_state(col);
    }

    std::string chessgame::check_state()
    {
        return m_board.check_state();
    }

    // Lobby or Administrative Functions
    error_e chessgame::remove_piece(coord_s p0)
    {
        if (m_board.remove(p0) == e_none)
        {
            signal_refresh_board();
            return e_none;
        }
        return e_removing;
    }

    error_e chessgame::add_piece(coord_s p0, chesspiece &p1)
    {
        if (m_board.add(p0, p1) == e_none)
        {
            signal_refresh_board();
            return e_none;
        }
        return e_adding;
    }

    error_e chessgame::rewind_game(int move_no)
    {
        int idx = move_no - 1;
        if (idx == turnno())
            return e_none;
        if ((idx < -1) || (idx >= turnno()))
            return e_rewind_failed;
        if (idx >= 0)
        {
            m_board.copy(m_turn[idx].b);
            m_turn.resize(idx + 1);
        }
        else
        {
            m_turn.clear();
            m_board.new_board();
        }
        signal_refresh_board();
        return e_none;
    }

    error_e chessgame::new_game(std::map<color_e, std::shared_ptr<chessplayer>> players, int skill)
    {
        if (players.count(c_white))
            _register(players[c_white], c_white);
        else
            _register(_default_computer_player(skill), c_white);
        if (players.count(c_black))
            _register(players[c_black], c_black);
        else
            _register(_default_computer_player(skill), c_black);
        m_state = play_e;
        m_win_color = c_none;
        m_board.new_board();
        m_turn.clear();
        m_board.trace("cg:new_game");
        signal_refresh_board();
        signal_on_turn();
        return e_none;
    }

    error_e chessgame::load_game(std::string filename)
    {
        try
        {
            std::ifstream is;
            is.open(filename, std::ios::binary | std::ios::in);
            if (!is.is_open())
                return e_loading;

            char header[] = "SENTINEL_CHESS";
            int headl = strlen(header);
            is.read(header, headl);
            if (strncmp(header, "SENTINEL_CHESS", headl))
                return e_loading;

            is.read((char *)&m_white_type, sizeof(m_white_type));
            is.read((char *)&m_black_type, sizeof(m_black_type));
            is.read((char *)&m_state, sizeof(m_state));
            is.read((char *)&m_win_color, sizeof(m_win_color));
            int num_turns = 0;
            is.read((char *)&num_turns, sizeof(num_turns));
            m_turn.clear();
            // If there are no turns ... we will just load the board
            chessturn_s t;
            for (int i = 0; i < num_turns; i++)
            {
                if (t.load(is) != e_none)
                {
                    is.close();
                    return e_loading;
                }
                m_turn.push_back(t);
            }
            if (m_board.load(is) != e_none)
                return e_loading;
            is.close();
            signal_refresh_board();
            signal_on_turn();
            return e_none;
        }
        catch (const std::exception &)
        {
            return e_loading;
        }
    }

    error_e chessgame::save_game(std::string filename)
    {
        try
        {
            std::ofstream os;
            os.open(filename, std::ios::binary | std::ios::out);

            char header[] = "SENTINEL_CHESS";
            os.write(header, strlen(header));

            os.write((char*)&m_white_type, sizeof(m_white_type));
            os.write((char*)&m_black_type, sizeof(m_black_type));
            os.write((char*)&m_state, sizeof(m_state));
            os.write((char*)&m_win_color, sizeof(m_win_color));
            int num_turns = turnno();
            os.write((char*)&num_turns, sizeof(num_turns));
            for (int i = 0; i < num_turns; i++)
            {
                if (m_turn[i].save(os) != e_none)
                {
                    os.close();
                    return e_saving;
                }
            }
            if (m_board.save(os) != e_none)
            {
                os.close();
                return e_saving;
            }
            os.close();
            return e_none;
        }
        catch (const std::exception&)
        {
            return e_saving;
        }
    }

    error_e chessgame::load_xfen(std::string contents)
    {
        error_e ret = m_board.load_xfen(contents);
        signal_refresh_board();
        return ret;
    }

    std::string chessgame::save_xfen()
    {
        return m_board.save_xfen();
    }

    error_e chessgame::_register(std::shared_ptr<chessplayer> pplayer, color_e color)
    {
        // Called by the lobby or game manager, the factory
        // has created the player object and it is being assigned
        // a slot
        std::lock_guard<std::mutex> guard(m_mutex);
        if (pplayer == NULL)
            return e_invalid_player;
        if (m_players.count(color) == 0)
        {
            if (color == c_white)
                m_white_type = pplayer->playertype();
            else if (color == c_black)
                m_black_type = pplayer->playertype();
            m_players[color] = pplayer;
            pplayer->_register(this, color);
            return e_none;
        }
        return e_player_already_registered;
    }

    error_e chessgame::_unregister(color_e color)
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        m_players.erase(color);
        if (color == c_white)
            m_white_type = t_none;
        else if (color == c_black)
            m_black_type = t_none;
        return e_none;
    }

    error_e chessgame::end_game(game_state_e end_state, color_e win_color)
    {
        m_state = end_state;
        m_win_color = win_color;
        signal_on_end();
        return e_none;
    }

    // Signallers and Private Functions
    std::shared_ptr<chessplayer> chessgame::_default_computer_player(int skill)
    {
        return std::shared_ptr<chesscomputer>(new chesscomputer("Computer", skill));
    }

    void chessgame::signal_refresh_board()
    {
        int n = turnno();
        for (const auto &kv : m_players)
            kv.second->signal_refresh_board(n, m_board);
    }

    void chessgame::signal_on_consider(move_s &m, color_e c, int p)
    {
        for (const auto &kv : m_players)
            kv.second->signal_on_consider(m, c, p);
    }

    void chessgame::signal_on_move(move_s &m, color_e c)
    {
        int n = turnno();
        for (const auto &kv : m_players)
            if (!kv.second->is(c))
                kv.second->signal_on_move(n, m, c);
    }

    void chessgame::signal_on_turn()
    {
        int n = turnno();
        color_e c = m_board.turn_color();
        bool ch = m_board.check_state(c);
        for (const auto &kv : m_players)
            if (kv.second->is(c))
                kv.second->signal_on_turn(n, ch, m_board);
    }

    void chessgame::signal_on_end()
    {
        for (const auto &kv : m_players)
            kv.second->signal_on_end(m_state, m_win_color);
    }

}