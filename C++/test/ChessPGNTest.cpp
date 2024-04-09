#include "gtest/gtest.h"

#include "chesscommon.h"
#include "chesspgn.h"
#include "chesslobby.h"
#include "PGNTest.h"

using namespace chess;
using namespace testing;

class ChessPGNTest : public Test
{
public:
    ChessPGNTest() { ; }
    ~ChessPGNTest() { ; }

    std::string test_file(std::string orig)
    {
        return fix_path("..\\..\\..\\test\\TestData\\PGN\\" + orig);
    }

protected:
    void SetUp() override { ; }
    // Sets up the test fixture.
    void TearDown() override { ; }
};

TEST_F(ChessPGNTest, LoadTest)
{
    chesspgn p;
    std::string errextra;
    EXPECT_EQ(e_none, p.load(test_file("00000001.pgn"), errextra));
    EXPECT_EQ("", errextra);
    EXPECT_EQ("Rated Classical game", p.event());
    EXPECT_EQ("BFG9k", p.white());
    EXPECT_EQ("mamalak", p.black());
    EXPECT_EQ(1639, p.whiteelo());
    EXPECT_EQ(1403, p.blackelo());
    EXPECT_EQ("C00", p.eco());
    EXPECT_EQ("French Defense: Normal Variation", p.opening());
    EXPECT_EQ("1-0", p["Result"]);
    EXPECT_EQ("2012.12.31", p["UTCDate"]);
    EXPECT_EQ("23:01:03", p["UTCTime"]);
    EXPECT_EQ("600+8", p["TimeControl"]);
    EXPECT_EQ("Normal", p["Termination"]);
    EXPECT_EQ(c_white, p.win_color());
    EXPECT_EQ(checkmate_e, p.game_state());
    std::string expected_moves = "1. e4 e6 2. d4 b6 3. a3 Bb7 4. Nc3 Nh6 5. Bxh6 gxh6 6. Be2 Qg5 7. Bg4 h5 "
                                 "8. Nf3 Qg6 9. Nh4 Qg5 10. Bxh5 Qxh4 11. Qf3 Kd8 12. Qxf7 Nc6 13. Qe8# 1-0";
    EXPECT_EQ(expected_moves, p.moves_str());
}

TEST_F(ChessPGNTest, SaveTest)
{
    chesspgn p;
    std::string errextra;
    EXPECT_EQ(e_none, p.load(test_file("00000001.pgn"), errextra));
    EXPECT_EQ(e_none, p.save("s0000001.pgn"));
    chesspgn q;
    EXPECT_EQ(e_none, q.load("s0000001.pgn", errextra));
    EXPECT_EQ("", errextra);
    EXPECT_EQ("Rated Classical game", q.event());
    EXPECT_EQ("BFG9k", q.white());
    EXPECT_EQ("mamalak", q.black());
    EXPECT_EQ(1639, q.whiteelo());
    EXPECT_EQ(1403, q.blackelo());
    EXPECT_EQ("C00", q.eco());
    EXPECT_EQ("French Defense: Normal Variation", q.opening());
    EXPECT_EQ("1-0", q["Result"]);
    EXPECT_EQ("2012.12.31", q["UTCDate"]);
    EXPECT_EQ("23:01:03", q["UTCTime"]);
    EXPECT_EQ("600+8", q["TimeControl"]);
    EXPECT_EQ("Normal", q["Termination"]);
    EXPECT_EQ(c_white, q.win_color());
    EXPECT_EQ(checkmate_e, q.game_state());
    std::string expected_moves = "1. e4 e6 2. d4 b6 3. a3 Bb7 4. Nc3 Nh6 5. Bxh6 gxh6 6. Be2 Qg5 7. Bg4 h5 "
                                 "8. Nf3 Qg6 9. Nh4 Qg5 10. Bxh5 Qxh4 11. Qf3 Kd8 12. Qxf7 Nc6 13. Qe8# 1-0";
    EXPECT_EQ(expected_moves, q.moves_str());
    _unlink("s0000001.pgn");
}

TEST_F(ChessPGNTest, SaveLobbyTest)
{
    chesslobby l;
    std::string errextra;
    EXPECT_EQ(e_none, l.load_game(test_file("00000002.pgn"), errextra));
    EXPECT_EQ("", errextra);
    EXPECT_EQ(e_none, l.save_game("s0000002.pgn"));
    EXPECT_EQ(e_none, l.load_game("s0000002.pgn", errextra));
    std::map<color_e, std::string> player_names = l.player_names();
    EXPECT_EQ(2, player_names.size());
    EXPECT_EQ("Desmond_Wilson", player_names[c_white]);
    EXPECT_EQ("savinka59", player_names[c_black]);
    _unlink("s0000002.pgn");
}

TEST_F(ChessPGNTest, WikiLoad)
{
    chesspgn p;
    std::string errextra;
    EXPECT_EQ(e_none, p.load(test_file("wikialgnt.pgn"), errextra));
    EXPECT_EQ("", errextra);
    EXPECT_EQ("Third Rosenwald Trophy", p.event());
    EXPECT_EQ("Donald Byrne", p.white());
    EXPECT_EQ("Robert James Fischer", p.black());
    EXPECT_EQ(-1, p.whiteelo());
    EXPECT_EQ(-1, p.blackelo());
    EXPECT_EQ("D92", p.eco());
    EXPECT_EQ(c_black, p.win_color());
    EXPECT_EQ(checkmate_e, p.game_state());
}

TEST_F(ChessPGNTest, LobbyLoad)
{
    chesslobby l;
    std::string errextra;
    EXPECT_EQ(e_none, l.load_game(test_file("00000002.pgn"), errextra));
    EXPECT_EQ("", errextra);
    std::map<color_e, std::string> player_names = l.player_names();
    EXPECT_EQ(2, player_names.size());
    EXPECT_EQ("Desmond_Wilson", player_names[c_white]);
    EXPECT_EQ("savinka59", player_names[c_black]);
    // Check the game state also
    EXPECT_EQ(c_white, l.game()->win_color());
    EXPECT_EQ(forfeit_e, l.game()->state());
    // Now, do 00000009.pgn which ends on a checkmate
    EXPECT_EQ(e_none, l.load_game(test_file("00000009.pgn"), errextra));
    EXPECT_EQ("", errextra);
    player_names = l.player_names();
    EXPECT_EQ(2, player_names.size());
    EXPECT_EQ("BFG9k", player_names[c_white]);
    EXPECT_EQ("Sagaz", player_names[c_black]);
    // Check the game state also
    EXPECT_EQ(c_black, l.game()->win_color());
    EXPECT_EQ(checkmate_e, l.game()->state());
}

TEST_F(ChessPGNTest, KnightMoves)
{
    chesspgn p;
    std::string errextra;
    EXPECT_EQ(e_none, p.load(test_file("00000007.pgn"), errextra));
    EXPECT_EQ("", errextra);
}

TEST_F(ChessPGNTest, QueenPromoteCheck)
{
    chesspgn p;
    std::string errextra;
    EXPECT_EQ(e_none, p.load(test_file("00000012.pgn"), errextra));
    EXPECT_EQ("", errextra);
}

TEST_F(ChessPGNTest, CastleCheck)
{
    chesspgn p;
    std::string errextra;
    EXPECT_EQ(e_none, p.load(test_file("00000042.pgn"), errextra));
    EXPECT_EQ("", errextra);
}

TEST_F(ChessPGNTest, Abdusattorov)
{
    // multiline
    chesslobby l;
    std::string errextra;
    EXPECT_EQ(e_none, l.load_game(test_file("00000051.pgn"), errextra));
    EXPECT_EQ("", errextra);
    std::map<color_e, std::string> player_names = l.player_names();
    EXPECT_EQ(2, player_names.size());
    EXPECT_EQ("Abdusattorov,Nodirbek", player_names[c_white]);
    EXPECT_EQ("Alikulov,Elbek", player_names[c_black]);
    EXPECT_EQ(c_white, l.game()->win_color());
}

TEST_F(ChessPGNTest, Grenkeop)
{
    // multiline
    chesslobby l;
    std::string errextra;
    EXPECT_EQ(e_none, l.load_game(test_file("00000061.pgn"), errextra));
    EXPECT_EQ("", errextra);
    std::map<color_e, std::string> player_names = l.player_names();
    EXPECT_EQ(2, player_names.size());
    EXPECT_EQ("Gressmann, Moritz", player_names[c_white]);
    EXPECT_EQ("Saric, Ivan", player_names[c_black]);
    EXPECT_EQ(c_black, l.game()->win_color());
}

TEST_F(ChessPGNTest, BruteForceLoad)
{
    for (int i = 1; i <= cx_pgn_count; i++)
    {
        std::ostringstream ss;
        ss << std::setw(8) << std::setfill('0') << i;
        std::string pgnfile = ss.str() + ".pgn";
        {
            chesspgn p;
            std::string errextra;
            EXPECT_EQ(e_none, p.load(test_file(pgnfile), errextra));
            EXPECT_EQ("", errextra);
            EXPECT_EQ(pgn_final_xfen(i), p.xfen());
            // use to generate new XFENs from properly loaded
            // std::cout << "\"" << p.xfen() << "\", // " << pgnfile << std::endl;
        }
    }
}
