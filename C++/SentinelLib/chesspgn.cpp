#include "chesspgn.h"
#include "chessboard.h"
#include "chesspiece.h"

#include <fstream>
#include <iostream>

namespace chess
{

    chesspgn::chesspgn()
    {
        reset_tags();
    }

    void chesspgn::reset_tags()
    {
        // we do this to preserve order for a few tags which are almost
        // always given.
        m_tags.clear();
        write_tag("Event", "???");
        write_tag("Site", "???");
        write_tag("Date", "???");
        write_tag("UTCDate", "???");
        write_tag("UTCTime", "???");
        write_tag("Round", "???");
        write_tag("White", "???");
        write_tag("Black", "???");
        write_tag("WhiteElo", "???");
        write_tag("BlackElo", "???");
        write_tag("ECO", "???");
        write_tag("Opening", "???");
        write_tag("Result", "???");
    }

    std::string chesspgn::event()
    {
        return tag("Event");
    }

    std::string chesspgn::white()
    {
        return tag("White");
    }

    std::string chesspgn::black()
    {
        return tag("Black");
    }

    int chesspgn::whiteelo()
    {
        std::string elo = tag("WhiteElo");
        if ((elo == "") || (elo == "?"))
            return -1;
        return atoi(elo.c_str());
    }

    int chesspgn::blackelo()
    {
        std::string elo = tag("BlackElo");
        if ((elo == "") || (elo == "?"))
            return -1;
        return atoi(elo.c_str());
    }

    std::string chesspgn::eco()
    {
        return tag("ECO");
    }

    std::string chesspgn::opening()
    {
        return tag("Opening");
    }

    std::string chesspgn::moves_str()
    {
        return m_moves_str;
    }

    std::string chesspgn::xfen()
    {
        return m_xfen;
    }

    std::vector<chessmove> chesspgn::moves()
    {
        return m_moves;
    }

    color_e chesspgn::win_color()
    {
        std::string result = tag("Result");
        if (result == "1-0")
            return c_white;
        else if (result == "0-1")
            return c_black;
        return c_none;
    }

    game_state_e chesspgn::game_state()
    {
        std::string result = tag("Result");
        std::string termination = tag("Termination");
        color_e wc = win_color();
        if (wc == c_none)
        {
            // draw states, pick one
            return draw_stalemate_e;
        }
        else
        {
            // had winner
            std::string checkmate = "# " + result;
            if (ends_with(m_moves_str, checkmate))
                return checkmate_e;
            if (termination == "Time forfeit")
                return time_up_e;
            return forfeit_e;
        }
    }

    void chesspgn::set_final_state(game_state_e g, color_e wc)
    {
        std::string result = "1/2-1/2";
        std::string termination = "Normal";
        if (wc == c_white)
            result = "1-0";
        else if (wc == c_black)
            result = "0-1";
        if (g == time_up_e)
            termination = "Time forfeit";
        write_tag("Result", result);
        write_tag("Termination", termination);
    }

    error_e chesspgn::read_tag(std::string line)
    {
        size_t l = line.length();
        if (l == 0)
            return e_pgn_parse;
        if ((line[0] == '[') && (line[l - 1] = ']'))
        {
            std::string mid = trim(line.substr(1, l - 2));
            l = line.find(' ');
            if (l == std::string::npos)
                return e_pgn_parse;
            std::string lhs = trim(mid.substr(0, l));
            std::string rhs = trim(mid.substr(l));
            // rhs must be quoted
            l = rhs.length();
            if (l == 0)
                return e_pgn_parse;
            if ((rhs[0] == '"') && (rhs[l - 1] == '"'))
            {
                rhs = trim(rhs.substr(1, l - 2));
                write_tag(lhs, rhs);
                return e_none;
            }
            return e_pgn_parse;
        }
        return e_pgn_parse;
    }

    error_e chesspgn::load(std::string filename, std::string &errextra)
    {
        try
        {
            std::ifstream pf(filename);
            if (!pf.is_open())
            {
                errextra = "Unable to open file - " + filename;
                return e_loading;
            }
            std::string line;
            error_e err = e_none;

            reset_tags();

            m_moves.clear();
            m_comments.clear();
            m_moves_str = "";
            std::string move_buffer = "";

            bool has_moves = false;

            while (std::getline(pf, line))
            {
                line = trim(line);
                if (line == "")
                    continue;
                if (book_end(line, '[', ']'))
                {
                    if (move_buffer != "")
                        break; // may be multi-PGN file.
                    err = read_tag(line);
                    if (err != e_none)
                    {
                        errextra = "Unable to parse tag: " + line;
                        return err;
                    }
                }
                else
                {
                    if (move_buffer != "")
                        move_buffer += "\r\n";
                    move_buffer += line;
                }
            }
            if (move_buffer != "")
            {
                if (!ends_with(move_buffer, tag("Result")))
                    return e_loading;
                std::string stripped_moves;
                error_e err = strip_comments(move_buffer, errextra);
                if (err == e_none)
                    err = read_pgn_moves(m_moves_str, m_moves, m_xfen, errextra);
                if (err != e_none)
                {
                    // remove
                    std::cout << "chesspgn::load error " << errextra << std::endl;
                    return err;
                }
                if (m_moves.size() == 0)
                    return e_loading;
                m_moves_str = move_buffer; // preserve original format
            }
            return e_none;
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << '\n';
            return e_loading;
        }
    }

    void append_comment(int idx, std::string comment, std::map<int, std::string> &comments)
    {
        if (comments.count(idx))
            comments[idx] = comments[idx] + " " + trim(comment);
        else
            comments[idx] = trim(comment);
    }

    void append_move(std::string word, std::string &move_str)
    {
        if (move_str != "")
            move_str += ' ';
        move_str += word;
    }

    error_e parse_comment(std::string &fwd_buffer, int turn_no, std::string &moves_str, std::map<int, std::string> &comments)
    {
        std::string token = std::to_string(turn_no) + ".";
        size_t t1 = fwd_buffer.find(token);
        if (t1 == std::string::npos)
        {
            fwd_buffer = "";
            return e_pgn_parse;
        }

        std::string token_term = std::to_string(turn_no) + "...";
        std::string next_token = std::to_string(turn_no + 1) + ".";

        std::string move_str = "";
        std::string comment = "";
        std::string word = "";

        size_t t2 = fwd_buffer.rfind(" " + next_token);
        size_t t3 = fwd_buffer.rfind("\n" + next_token);
        if (t2 == std::string::npos)
            t2 = t3;
        if (t3 != std::string::npos)
            if (t3 > t2)
                t2 = t3;

        size_t buffer_max = (t2 == std::string::npos) ? fwd_buffer.length() : t2;
        int inc = 0;
        int sc = (turn_no - 1) * 2 - 1;

        for (size_t i = 0; i < buffer_max; i++)
        {
            char c = fwd_buffer[i];
            if (c == '\r')
                continue;
            if (c == '\n')
                c = ' ';
            if ((c == '{') || (c == '('))
            {
                inc++;
            }
            else if ((c == '}') || (c == ')'))
            {
                inc--;
                if (inc < 0)
                    return e_pgn_parse;
                if (inc == 0)
                {
                    comment += c;
                    append_comment(sc, comment, comments);
                    comment = "";
                    continue;
                }
            }
            if (inc)
            {
                comment += c;
            }
            else
            {
                if (c == ' ')
                {
                    if (word != "")
                    {
                        char wc = word[0];
                        if (word == next_token)
                        {
                            t2 = i - next_token.length() - 1;
                            if (fwd_buffer[i - 1] == '\r')
                                t2--;
                            word = "";
                            break;
                        }
                        if (word == token_term)
                        {
                            append_comment(sc, word, comments);
                        }
                        else if (wc == '$')
                        {
                            append_comment(sc, word, comments);
                        }
                        else
                        {
                            append_move(word, move_str);
                            if (((wc >= '0') && (wc <= '9')) ||
                                ((wc >= 'A') && (wc <= 'Z')) ||
                                ((wc >= 'a') && (wc <= 'z')))
                                sc++;
                            if (sc > turn_no * 2 + 1)
                                return e_pgn_parse;
                        }
                        word = "";
                    }
                }
                else
                {
                    word += c;
                }
            }
        }

        if (word != "")
        {
            char wc = word[0];
            if (word == token_term)
                append_comment(sc, word, comments);
            else if (wc == '$')
                append_comment(sc, word, comments);
            else
                append_move(word, move_str);
        }

        // add to the whole kit n kboodle
        append_move(move_str, moves_str);

        if (t2 == std::string::npos)
            fwd_buffer = "";
        else
            fwd_buffer = fwd_buffer.substr(t2 + 1);
        return e_none;
    }

    error_e chesspgn::strip_comments(std::string move_buffer, std::string &errextra)
    {
        if (move_buffer.find_first_of("{()}$") == std::string::npos)
        {
            m_comments.clear();
            m_moves_str = string_replace(move_buffer, "\r\n", " ");
            return e_none;
        }

        // now we want to split in to a vector of 'lines' or move sets
        std::string fwd_buffer = move_buffer;
        std::string moves_str = "";
        std::map<int, std::string> comments;
        error_e err = e_none;
        int turn_no = 0;
        while (fwd_buffer != "")
        {
            err = parse_comment(fwd_buffer, ++turn_no, moves_str, comments);
            if (err != e_none)
                break;
        }

        if (err == e_none)
        {
            m_moves_str = moves_str;
            m_comments = comments;
        }
        return err;
    }

    std::string disambiguate_piece(chessboard &b, coord_s p0, std::vector<chessmove> &m, coord_s p1)
    {
        std::string ret = "";
        unsigned char my_piece = b.get(p0) & piece_and_color_mask;
        for (size_t i = 0; i < m.size(); i++)
        {
            unsigned other_piece = b.get(m[i].p0) & piece_and_color_mask;
            if ((my_piece == other_piece) && (m[i].p0 != p0) && (m[i].p1 == p1))
            {
                std::string ret = "";
                if (m[i].p0.x != p0.x)
                    ret = 'a' + p0.x;
                else if (m[i].p0.y != p0.y)
                    ret = '1' + p0.y;
                break;
            }
        }
        return ret;
    }

    std::string split_lines(std::string orig, size_t width)
    {
        if (orig.find_first_of('\n') != std::string::npos)
            return orig;
        std::string word;
        std::string multiline;
        size_t last_line = 0;
        size_t i = 0;
        for (; i < orig.length(); i++)
        {
            char c = orig[i];
            if (c == ' ')
            {
                size_t cp = multiline.length() - last_line;
                size_t wl = word.length();
                bool newline = cp + wl + 1 >= width;
                if (newline)
                {
                    multiline += "\r\n";
                    last_line = multiline.length();
                }
                else
                {
                    if (multiline != "")
                        multiline += " ";
                }
                multiline += word;
                word = "";
            }
            else
            {
                word += c;
            }
        }
        if (word != "")
        {
            size_t cp = multiline.length() - last_line;
            size_t wl = word.length();
            if (cp + wl + 1 >= width)
            {
                multiline += "\r\n";
            }
            else
            {
                if (multiline != "")
                    multiline += " ";
            }
            multiline += word;
        }

        return multiline;
    }

    void append_moves_str(std::string val, std::string &moves_str)
    {
        if (moves_str != "")
            moves_str += " ";
        moves_str += val;
    }

    error_e chesspgn::write_moves(std::vector<chessmove> move_vec)
    {
        chessboard b;
        b.load_xfen(c_open_board);
        color_e tc = b.turn_color();

        int ft = 0;

        std::string moves_str = "";

        // Heading comment?
        if (m_comments.count(-1))
            moves_str = m_comments[-1];

        for (chessmove m : move_vec)
        {
            int idx = ft / 2 + 1;
            if ((ft++ % 2) == 0)
                append_moves_str(std::to_string(idx) + ".", moves_str);
            append_moves_str(move_str(m, b), moves_str);
            if (m_comments.count(ft))
                append_moves_str(m_comments[ft], moves_str);
            if (moves_str == "")
                return e_saving;
        }

        if (game_state() == checkmate_e)
        {
            if (!ends_with(moves_str, "#"))
                moves_str += "#";
        }
        if (win_color() == c_white)
            moves_str += " 1-0";
        else if (win_color() == c_black)
            moves_str += " 0-1";
        else
            moves_str += " 1/2-1/2";

        m_moves_str = split_lines(moves_str, 80);
        return e_none;
    }

    error_e chesspgn::save(std::string filename)
    {
        try
        {
            std::ofstream pf(filename, std::ios::binary);
            if (!pf.is_open())
                return e_saving;

            for (size_t i = 0; i < m_tags.size(); i++)
            {
                std::pair<std::string, std::string> pair = m_tags[i];
                std::string value = pair.second;
                if (value != "")
                    if (value != "???")
                        pf << "[" << pair.first << " \"" << pair.second << "\"]\r\n";
            }

            pf << "\r\n";

            pf << m_moves_str;

            pf.close();
            return e_none;
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << '\n';
            return e_saving;
        }
    }

    chessplayerdata chesspgn::get_playerdata(color_e col)
    {
        chessplayerdata pd;
        std::string pref = (col == c_white) ? "White" : "Black";
        pd.username = (col == c_white) ? white() : black();
        pd.elo = (col == c_white) ? whiteelo() : blackelo();
        pd.ptype = t_human;
        return pd;
    }

    error_e load_move(std::string s, color_e tc, chessboard &b, chessmove &m, std::string &errextra)
    {
        error_e err = str_move(s, tc, b, m);
        if (err != e_none)
        {
            errextra = "Move parse error - " + s;
            return err;
        }
        m = b.attempt_move(tc, m);
        if (m.error != e_none)
        {
            errextra = "Move execute error - " + s;
            return err;
        }
        return e_none;
    }

    bool terminal(std::string move)
    {
        if ((move == "") || (move == "*"))
            return true;
        if ((move[0] == '1') || (move[0] == '0'))
        {
            if (starts_with(move, "1/2-"))
                return true;
            if (starts_with(move, "0-"))
                return true;
            if (starts_with(move, "1-"))
                return true;
        }
        return false;
    }

    // this function parses a pgn sequence from a line or lines
    error_e read_pgn_moves(std::string moves_str, std::vector<chessmove> &move_vec, std::string &xfen, std::string &errextra)
    {
        chessboard b;
        b.load_xfen(c_open_board);
        color_e tc = b.turn_color();

        move_vec.clear();
        xfen = c_open_board;
        errextra = "";

        std::string moves = moves_str;

        // We want to move through the file in move pairs.
        int ft = 0;
        std::string fts = std::to_string(++ft) + ".";
        size_t q = moves.find(fts);
        if (q == std::string::npos)
            return e_none;
        moves = trim(moves.substr(q + fts.length()));

        error_e err = e_none;
        while (moves != "")
        {
            std::string move_pair = moves;
            std::string fts = std::to_string(++ft) + ".";
            q = moves.find(fts);
            if (q != std::string::npos)
            {
                move_pair = trim(moves.substr(0, q));
                moves = trim(moves.substr(q + fts.length()));
            }
            else
            {
                moves = "";
            }
            std::vector<std::string> pair = split_string(move_pair, ' ');
            if ((pair.size() == 0) || (pair.size() > 3))
            {
                err = e_loading;
                errextra = "Full move sequence error";
                break;
            }
            std::string white = trim(pair[0]);
            if (terminal(white))
                break;
            if (tc != c_white)
            {
                err = e_out_of_turn;
                break;
            }
            chessmove m;
            err = load_move(white, tc, b, m, errextra);
            if (err != e_none)
                break;
            move_vec.push_back(m);
            tc = b.turn_color();
            std::string black;
            if (pair.size() > 1)
                black = pair[1];
            if (!terminal(black))
            {
                m.invalidate();
                err = load_move(black, tc, b, m, errextra);
                if (err != e_none)
                    break;
                move_vec.push_back(m);
                tc = b.turn_color();
            }
        }

        if (ft > 0)
        {
            xfen = b.save_xfen();
            if (errextra != "")
                errextra += " at " + std::to_string(ft) + ".";
        }
        return err;
    }
}