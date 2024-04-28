#include "chessgame.h"
#include "chesscomputer.h"
#include "chessclock.h"
#include "chessengine.h"
// #include <iostream>

namespace chess
{

    chessgame::chessgame()
    {
        m_state = none_e;
        m_win_color = c_none;
        m_play_pos = -1;
        m_puzzle = false;
        m_hints = 0;
        m_awarded = false;
        m_puzzle_mult = 1.0f;
    }

    chessgame::~chessgame()
    {
        remove_clock();
    }

    chessboard chessgame::board()
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        return m_board;
    }

    chessboard chessgame::board(int16_t t)
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        int turn = m_puzzle ? t + 1 : t;
        if ((turn > 0) && (turn < playmax()))
            return m_turn[turn - 1].b;
        chessboard b;
        b.load_xfen(m_init_board);
        return b;
    }

    game_state_e chessgame::state()
    {
        return m_state;
    }

    std::string chessgame::eco()
    {
        // depends on state of eco database and move history
        if (m_puzzle)
            return tag("puzzle_id");
        else
            return m_open_filter.eco();
    }

    std::string chessgame::open_title()
    {
        // depends on state of eco database and move history
        if (m_puzzle)
            return tag("puzzle_open");
        else
            return m_open_filter.title();
    }

    int chessgame::possible_opening_count()
    {
        // depends on state of eco database and move history
        return (int)m_open_filter.possible_opening_count();
    }

    error_e chessgame::next_opening_moves(color_e col, std::string eco, std::vector<chessmove> &m)
    {
        // depends on state of eco database and move history
        return m_open_filter.next_opening_moves(col, eco, m);
    }

    void chessgame::narrow_moves()
    {
        std::vector<chessmove> mv = moves();
        m_open_filter.narrow(mv);
        // std::cout << "ECO: " << m_open_filter.eco() << " Title: " << m_open_filter.title() << std::endl;
    }

    color_e chessgame::turn_color()
    {
        return m_board.turn_color();
    }

    color_e chessgame::win_color()
    {
        return m_win_color;
    }

    chessturn chessgame::play_turn()
    {
        if ((m_turn.size() == 0) || (m_play_pos < 0))
        {
            chessturn m;
            m.t = -1;
            m.b.load_xfen(m_init_board);
            m.c = m.b.turn_color();
            m.ch = m.b.check_state(m.c);
            m.g = play_e;
            return m;
        }
        return m_turn[m_play_pos];
    }

    int16_t chessgame::playmax()
    {
        return (int16_t)m_turn.size();
    }

    int16_t chessgame::playno()
    {
        return (int16_t)m_play_pos + 1;
    }

    game_state_e chessgame::is_game_over(color_e col, chessmove m)
    {
        game_state_e g = m_state;
        if (g == play_e)
        {
            // Defensive Checkmate
            color_e winner_col = other(col);
            if (!m.is_valid())
            {
                // Player was unable to obtain a move
                // or an erroneous move was set.  Let's check
                m.mate = true;
                m.check = check_state(col);
                if (!m.check)
                    m.mate = false;
            }
            // Offensive Checkmate
            if (!m.mate)
            {
                color_e opponent = other(col);
                m.mate = true;
                m.check = check_state(opponent);
                if (!m.check)
                {
                    m.mate = false;
                }
                else
                {
                    // Check all moves
                    std::vector<chessmove> pm = possible_moves(opponent);
                    for (size_t i = 0; i < pm.size(); i++)
                    {
                        chessboard b(m_board);
                        chessmove m0 = b.attempt_move(opponent, pm[i]);
                        if (m0.is_valid())
                        {
                            m.mate = false;
                            break;
                        }
                    }
                    if (m.mate)
                        winner_col = col;
                }
            }
            // Conclusion
            if (m.mate)
            {
                if (m.check)
                {
                    g = checkmate_e;
                    m_win_color = winner_col;
                }
                else
                    g = draw_stalemate_e;
            }
            // Fivefold Repetition
            if (prev_position_count() >= 4)
            {
                g = draw_fivefold_e;
                m_win_color = c_none;
            }
            // Puzzle mode
            if (m.error == e_incorrect_move)
            {
                if (m_hints <= 0)
                {
                    m_win_color = other(col);
                    g = puzzle_solution_e;
                }
            }
        }
        return g;
    }

    void chessgame::clock_remaining(int32_t &wt, int32_t &bt)
    {
        wt = 0;
        bt = 0;
        if (mp_clock)
            mp_clock->time_remaining(wt, bt);
    }

    error_e chessgame::forfeit(color_e col)
    {
        return end_game(forfeit_e, other(col));
    }

    error_e chessgame::puzzle_solved(color_e col)
    {
        return end_game(puzzle_solution_e, other(col));
    }

    error_e chessgame::move(color_e col, chessmove m0)
    {
        std::unique_lock<std::mutex> guard(m_mutex);
        if (m_state != play_e)
        {
            guard.unlock();
            return e_invalid_game_state;
        }
        if (col == m_board.turn_color())
        {
            chessmove m;
            if (m_puzzle)
            {
                m = m0;
                std::map<int16_t, chessmove> moves = player_moves(col);
                if (moves.count(m_play_pos + 1) == 0)
                    m.error = e_incorrect_move;
                else
                {
                    chessmove pm = moves[m_play_pos + 1];
                    if ((pm.p0 != m0.p0) || (pm.p1 != m0.p1))
                        m.error = e_incorrect_move;
                }
                if (m.error == e_incorrect_move)
                {
                    // Consume a hint
                    if (m_hints > 0)
                    {
                        m_hints--;
                        m_puzzle_mult *= 0.5f;
                        guard.unlock();
                        return m.error;
                    }
                }
            }
            if (m.error == e_none)
                m = m_board.attempt_move(col, m0);
            guard.unlock();
            if (m.error == e_none)
                push_new_turn(m);
            set_state(is_game_over(col, m));
            return m.error;
        }
        guard.unlock();
        return e_out_of_turn;
    }

    error_e chessgame::move(color_e col, coord_s p0, coord_s p1, piece_e promote)
    {
        chessmove m(p0, p1, promote);
        return move(col, m);
    }

    error_e chessgame::move(color_e col, std::string s)
    {
        if (col != turn_color())
            return e_out_of_turn;
        chessmove m0;
        error_e err = str_move(s, col, m_board, m0);
        if (err == e_none)
            return move(col, m0);
        return err;
    }

    std::vector<chessmove> chessgame::possible_moves(color_e col)
    {
        return m_board.possible_moves(col);
    }

    std::map<int16_t, chessmove> chessgame::player_moves(color_e col)
    {
        // as far as TURN is concerned, the records are whose turn
        // it IS rather than WAS, and the move is 'WAS'
        // So we search for the reverse color.
        std::map<int16_t, chessmove> ret;
        int16_t num_turns = playmax();
        for (int16_t i = 0; i < num_turns; i++)
            if (m_turn[i].c == other(col))
                ret[m_turn[i].t] = m_turn[i].m;
        return ret;
    }

    bool chessgame::puzzle()
    {
        return m_puzzle;
    }

    int chessgame::hints()
    {
        return m_hints;
    }

    chessmove chessgame::hint()
    {
        if ((m_hints > 0) && (m_play_pos < playmax()))
        {
            m_hints--;
            m_puzzle_mult *= 0.5f;
            return m_turn[m_play_pos + 1].m;
        }
        chessmove m;
        m.error = e_invalid_move;
        return m;
    }

    std::string chessgame::hintstr()
    {
        if ((m_hints > 0) && (m_play_pos < playmax()))
        {
            m_hints--;
            m_puzzle_mult *= 0.5f;
            return move_str(m_turn[m_play_pos + 1].m);
        }
        return "No Hint Available";
    }

    bool chessgame::awarded()
    {
        return m_awarded;
    }

    void chessgame::potential_points(color_e col, int32_t &win, int32_t &lose, int32_t &draw)
    {
        std::map<color_e, int32_t> elo;
        if (has_tag("WhiteElo"))
            elo[c_white] = atoi(tag("WhiteElo").c_str());
        if (has_tag("BlackElo"))
            elo[c_black] = atoi(tag("BlackElo").c_str());
        calc_elo_points(col, elo, win, lose, draw);
        if (m_puzzle)
            win = (int32_t)(((float)win) * m_puzzle_mult);
    }

    int32_t chessgame::award_points(color_e col)
    {
        int32_t win = 0;
        int32_t lose = 0;
        int32_t draw = 0;
        potential_points(col, win, lose, draw);
        int32_t to_award = m_win_color ? win : lose;
        if (m_state >= draw_stalemate_e)
            to_award = draw;
        if (to_award != 0)
        {
            std::string colbase = color_str(col);
            if (has_tag(colbase + "Type"))
            {
                chessplayertype_e ptype = playertypefromstring(tag(colbase + "Type"));
                if (ptype == t_human)
                {
                    std::string pguid = tag(colbase + "Guid");
                    if (pguid != "")
                        chessengine::hub_update_points(pguid, to_award, m_puzzle);
                }
            }
        }
        return to_award;
    }

    std::string chessgame::title()
    {
        return tag("Event");
    }

    void chessgame::set_title(std::string t)
    {
        write_tag("Event", t);
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

    void chessgame::set_turn_to(int idx, bool activate)
    {
        if ((idx != m_play_pos) || (activate))
        {
            bool turn_rw = (m_state == play_e && !m_puzzle);
            if ((idx >= 0) && (m_turn.size() > 0))
            {
                m_board.copy(m_turn[idx].b);
                if (turn_rw)
                    m_turn.resize(idx + 1);
                m_play_pos = idx;
            }
            else
            {
                m_board.load_xfen(m_init_board);
                if (turn_rw)
                    m_turn.clear();
                m_play_pos = -1;
            }
            if (turn_rw)
                refresh_board_positions();
        }
        signal_on_turn(activate);
    }

    error_e chessgame::rewind_game()
    {
        if (m_state == play_e)
            return e_play_not_paused;
        int idx = m_play_pos - 1;
        if (idx < -1)
            return e_rewind_failed;
        set_turn_to(idx);
        return e_none;
    }

    error_e chessgame::advance_game()
    {
        if (m_state == play_e)
            return e_play_not_paused;
        int idx = m_play_pos + 1;
        if (idx >= playmax())
            return e_advance_failed;
        set_turn_to(idx);
        return e_none;
    }

    error_e chessgame::goto_turn(int turn_no)
    {
        if (m_state == play_e)
            return e_play_not_paused;
        int idx = turn_no - 1;
        if ((idx < -1) || (idx >= playmax()))
            return e_advance_failed;
        set_turn_to(idx);
        return e_none;
    }

    error_e chessgame::play_game()
    {
        if (m_state != play_e)
        {
            // accomplishes a trim
            m_win_color = c_none;
            set_state(play_e);
            set_turn_to(m_play_pos, true);
        }
        return e_none;
    }

    error_e chessgame::pause_game()
    {
        if (m_state != idle_e)
        {
            set_state(idle_e);
        }
        return e_none;
    }

    error_e chessgame::new_game(std::string title, const chessclock_s &clock)
    {
        m_win_color = c_none;
        m_init_board = c_open_board;
        m_puzzle = false;
        m_awarded = false;
        remove_tag("puzzle_id");
        remove_tag("puzzle_open");
        m_board.load_xfen(m_init_board);
        m_turn.clear();
        m_play_pos = -1;
        write_tag("Event", title);
        m_open_filter.reset();
        add_clock(clock);
        refresh_board_positions();
        set_state(play_e, true);
        signal_on_turn();
        return e_none;
    }

    error_e chessgame::load_chs(json &jsonf)
    {
        try
        {
            if (jsonf.is_null())
                return e_loading;
            m_state = str_game_state(jsonf["state"]);
            m_win_color = str_color(jsonf["win_color"]);

            m_open_filter.reset();
            add_clock(chessclock::load(jsonf["Clock"], this));

            auto turns = jsonf["turns"];
            m_turn.clear();

            chessturn t;
            for (auto turn : turns)
            {
                if (t.load(turn) != e_none)
                    return e_loading;
                m_turn.push_back(t);
            }

            JSON_LOAD(jsonf, "init_board", m_init_board, c_open_board);
            JSON_LOAD(jsonf, "puzzle", m_puzzle, false);
            JSON_LOAD(jsonf, "puzzle_mult", m_puzzle_mult, 1.0f);
            JSON_LOAD(jsonf, "hints", m_hints, 0);
            JSON_LOAD(jsonf, "awarded", m_awarded, false);

            reset_tags();

            auto meta = json::object();
            JSON_LOADC(jsonf, "meta", meta);
            if (!meta.is_null())
            {
                for (auto el : meta.items())
                    write_tag(el.key(), el.value().dump());
            }

            clear_comments();

            auto commentsj = json::object();
            JSON_LOADC(jsonf, "comments", commentsj);
            if (!commentsj.is_null())
            {
                for (auto el : commentsj.items())
                {
                    int key = atoi(el.key().c_str());
                    comment(key, el.value().dump());
                }
            }

            m_play_pos = jsonf["play_pos"];
            if (m_play_pos < 0)
                m_play_pos = 0;
            if (m_play_pos > playmax() - 1)
                m_play_pos = playmax() - 1;

            auto board = jsonf["board"];
            if (m_board.load(board) != e_none)
                return e_loading;
            narrow_moves();
            refresh_board_positions();
            // when we load any game, if it's state
            // is play we turn to idle (review mode)
            set_state(idle_e, true);
            signal_on_turn();
            return e_none;
        }
        catch (const std::exception &)
        {
            return e_loading;
        }
    }

    error_e chessgame::load_puzzle(chesspuzzle &p)
    {
        chessboard b;
        error_e err = b.load_xfen(p.fen);
        if (err != e_none)
            return err;
        std::vector<std::string> moves = split_string(p.moves, ' ');
        if (moves.size() < 2)
            return e_loading;

        color_e tc = b.turn_color();
        std::vector<chessturn> turns;

        int16_t n = 0;
        chessturn t;

        for (std::string move : moves)
        {
            chessmove m;
            err = str_move(move, tc, b, m);
            if (err != e_none)
                return err;
            m = b.attempt_move(tc, m);
            if (m.error != e_none)
                return err;
            tc = b.turn_color();
            t.t = n++;
            t.b = b;
            t.c = tc;
            t.m = m;
            t.ch = b.check_state(tc);
            t.g = play_e;
            t.wc = c_none;
            turns.push_back(t);
        }
        mp_clock = nullptr;
        m_init_board = p.fen;
        m_turn = turns;
        m_play_pos = -1;
        m_board.load_xfen(p.fen);
        m_puzzle = true;
        m_puzzle_mult = 1.0f;
        m_awarded = false;
        m_hints = (int)turns.size() / 4;
        write_tag("Event", p.themes);
        write_tag("puzzle_id", p.puzzleid);
        write_tag("puzzle_open", p.openingtags);
        write_tag("puzzle_themes", p.themes);
        write_tag("puzzle_ratings", p.rating);
        // Save ELO of puzzle
        write_tag(color_str(turn_color()) + "Elo", p.rating);
        refresh_board_positions();
        set_state(play_e, true);
        signal_on_turn();
        return e_none;
    }

    error_e chessgame::load_pgn(chesspgn &p)
    {
        chessboard b;
        error_e err = b.load_xfen(c_open_board);
        if (err != e_none)
            return err;

        color_e tc = b.turn_color();
        std::vector<chessturn> turns;
        m_open_filter.reset();
        m_awarded = true;

        int16_t n = 1;
        chessturn t;

        for (chessmove m : p.moves())
        {
            m = b.attempt_move(tc, m);
            if (m.error != e_none)
                return err;
            tc = b.turn_color();
            t.t = n++;
            t.b = b;
            t.c = tc;
            t.m = m;
            t.ch = b.check_state(tc);
            t.g = play_e;
            t.wc = c_none;
            turns.push_back(t);
        }

        mp_clock = nullptr;
        m_init_board = c_open_board;
        m_turn = turns;

        m_play_pos = -1;
        m_board.load_xfen(c_open_board);
        m_puzzle = false;

        copy_from(p);
        refresh_board_positions();

        // update the last turn.
        int last_turn = playmax();
        game_state_e detect_state = idle_e;
        if (last_turn >= 1)
        {
            // We want to replay the last move for state sake and load position
            goto_turn(last_turn);
            chessturn lt = turns[last_turn - 1];
            m_state = play_e;
            detect_state = is_game_over(other(lt.c), lt.m);
            if (m_win_color == c_none)
                m_win_color = lt.wc = p.win_color();
            if (detect_state == play_e)
                detect_state = lt.g = p.game_state();
            turns[last_turn - 1] = lt;
        }
        narrow_moves();
        set_state(idle_e, true);
        // and set it.
        signal_on_turn();
        return e_none;
    }

    error_e chessgame::save_pgn(chesspgn &p)
    {
        write_tag("ECO", m_open_filter.eco());
        write_tag("Opening", m_open_filter.title());
        p.copy_from(*this);
        int last_turn = playmax();
        if (last_turn >= 1)
        {
            chessturn lt = m_turn[last_turn - 1];
            p.set_final_state(lt.g, lt.wc);
        }
        return p.write_moves(moves());
    }

    error_e chessgame::save_chs(json &jsonf)
    {
        try
        {
            jsonf["state"] = game_state_str(m_state);
            jsonf["win_color"] = color_str(win_color());

            auto turns = json::array();
            int16_t num_turns = playmax();
            for (int i = 0; i < num_turns; i++)
            {
                auto turn = json::object();
                if (m_turn[i].save(turn) != e_none)
                    return e_saving;
                turns.push_back(turn);
            }
            jsonf["turns"] = turns;

            if (mp_clock)
            {
                auto clock = json::object();
                mp_clock->save(clock);
                jsonf["Clock"] = clock;
            }

            jsonf["init_board"] = m_init_board;
            jsonf["puzzle"] = m_puzzle;
            jsonf["puzzle_mult"] = m_puzzle_mult;
            jsonf["hints"] = m_hints;
            jsonf["awarded"] = m_awarded;
            jsonf["play_pos"] = m_play_pos;

            write_tag("ECO", m_open_filter.eco());
            write_tag("Opening", m_open_filter.title());

            auto meta = json::object();

            for (auto meta_pair : m_tags)
                meta[meta_pair.first] = meta_pair.second;

            jsonf["meta"] = meta;

            auto commentsj = json::object();

            for (auto comment_pair : m_comments)
                commentsj[comment_pair.first] = comment_pair.second;

            jsonf["comments"] = commentsj;

            auto board = json::object();
            if (m_board.save(board) != e_none)
                return e_saving;

            jsonf["board"] = board;
            return e_none;
        }
        catch (const std::exception &)
        {
            return e_saving;
        }
    }

    error_e chessgame::load_xfen(std::string contents)
    {
        chessboard b;
        error_e ret = b.load_xfen(contents);
        if (ret != e_none)
            return ret;
        m_board.copy(b);
        m_win_color = c_none;
        if (mp_clock)
            mp_clock->clear();
        m_turn.clear();
        m_play_pos = -1;
        refresh_board_positions();
        // when we load any game, if it's state
        // is play we turn to idle (review mode)
        set_state(idle_e, true);
        signal_on_turn();
        return ret;
    }

    std::string chessgame::save_xfen()
    {
        return m_board.save_xfen();
    }

    void chessgame::add_clock(const chessclock_s &clock)
    {
        std::shared_ptr<chessclock> p_clock(new chessclock(clock, this));
        add_clock(p_clock);
    }

    void chessgame::add_clock(std::shared_ptr<chessclock> p_clock)
    {
        remove_clock();
        mp_clock = p_clock;
        if (mp_clock)
            listen(mp_clock);
    }

    void chessgame::remove_clock()
    {
        if (mp_clock)
            unlisten(mp_clock->id());
        mp_clock = nullptr;
    }

    error_e chessgame::listen(std::shared_ptr<chessgamelistener> plistener)
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        if (plistener == NULL)
            return e_invalid_listener;
        int id = plistener->id();
        mp_listeners[id] = plistener;
        return e_none;
    }

    error_e chessgame::unlisten(int id)
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        if (mp_listeners.count(id))
            mp_listeners.erase(id);
        return e_none;
    }

    error_e chessgame::end_game(game_state_e end_state, color_e win_color)
    {
        std::unique_lock<std::mutex> guard(m_mutex);
        m_win_color = win_color;
        guard.unlock();
        set_state(end_state, true);
        return e_none;
    }

    error_e chessgame::chat(std::string msg, color_e col)
    {
        signal_chat(msg, col);
        return e_none;
    }

    error_e chessgame::consider(chessmove m, color_e c, int8_t p)
    {
        signal_on_consider(m, c, p);
        return e_none;
    }

    void chessgame::set_state(game_state_e g, bool force_notify)
    {
        if ((g != m_state) || (force_notify))
        {
            m_state = g;
            if ((m_awarded == false) && (m_state >= terminate_e))
                signal_on_points();
            signal_on_state();
        }
    }

    chessturn chessgame::new_turn(chessmove m)
    {
        int16_t n = playmax() + 1;
        if ((m_turn.size() == 0) && (!m.is_valid()))
            n = 0;
        game_state_e g = state();
        color_e c = turn_color();
        bool ch = check_state(c);
        color_e wc = win_color();
        int32_t wt = 0;
        int32_t bt = 0;
        clock_remaining(wt, bt);
        return chess::new_turn(n, m, ch, m_board, c, g, wc, wt, bt);
    }

    std::vector<chessmove> chessgame::moves()
    {
        std::vector<chessmove> ret;
        for (int i = 0; i <= m_play_pos; i++)
            ret.push_back(m_turn[i].m);
        return ret;
    }

    void chessgame::push_new_turn(chessmove m)
    {
        if (m.is_valid())
        {
            if (!m_puzzle)
            {
                add_board_position();
                m_turn.push_back(new_turn(m));
                m_play_pos = (int)m_turn.size() - 1;
                narrow_moves();
            }
            else
            {
                if (m_play_pos < (int)m_turn.size() - 1)
                    m_play_pos++;
            }
            signal_on_turn();
        }
    }

    void chessgame::refresh_board_positions()
    {
        m_board_positions.clear();
        for (size_t i = 0; i < m_turn.size(); i++)
        {
            uint32_t curpos = m_turn[i].b.hash();
            int c = m_board_positions[curpos];
            m_board_positions[curpos] = c + 1;
        }
    }

    int chessgame::prev_position_count()
    {
        uint32_t curpos = m_board.hash();
        std::map<uint32_t, int>::iterator it = m_board_positions.find(curpos);
        if (it != m_board_positions.end())
            return it->second;
        return 0;
    }

    void chessgame::add_board_position()
    {
        uint32_t curpos = m_board.hash();
        int c = m_board_positions[curpos];
        m_board_positions[curpos] = c + 1;
    }

    void chessgame::signal_refresh_board()
    {
        int16_t n = playno();
        for (const auto &kv : mp_listeners)
            kv.second->signal_refresh_board(n, m_board);
    }

    void chessgame::signal_on_consider(chessmove m, color_e c, int8_t p)
    {
        for (const auto &kv : mp_listeners)
            kv.second->signal_on_consider(m, c, p);
    }

    void chessgame::signal_on_turn(bool refreshtime)
    {
        chessmove m;
        chessturn l = m_turn.size() > 0 ? play_turn() : new_turn(m);
        std::string cmt = comment(l.t + 1);
        if (refreshtime)
            clock_remaining(l.wt, l.bt);
        for (const auto &kv : mp_listeners)
            kv.second->signal_on_turn(l.t, l.m, l.ch, l.b, l.c, l.g, l.wc, l.wt, l.bt, cmt);
    }

    void chessgame::signal_on_state()
    {
        for (const auto &kv : mp_listeners)
            kv.second->signal_on_state(m_state, m_win_color);
    }

    void chessgame::signal_on_points()
    {
        m_awarded = true;
        int32_t wp = award_points(c_white);
        int32_t bp = award_points(c_black);
        for (const auto &kv : mp_listeners)
            kv.second->signal_on_points(wp, bp);
    }

    void chessgame::signal_chat(std::string msg, color_e c)
    {
        for (const auto &kv : mp_listeners)
            kv.second->signal_chat(msg, c);
    }
}