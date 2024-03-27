#include "gtest/gtest.h"

#include "chesscommon.h"
#include "chessboard.h"
#include "chessmove.h"

#include "XFENTest.h"

using namespace chess;
using namespace testing;

#include "nlohmann/json.hpp"

class ChessBoardTest : public Test
{
public:
    ChessBoardTest() { ; }
    ~ChessBoardTest() { ; }

protected:
    void SetUp() override { ; }
    // Sets up the test fixture.
    void TearDown() override { ; }
};

TEST_F(ChessBoardTest, BoardInitTest)
{
    chessboard b;
    EXPECT_EQ(e_none, b.load_xfen(cx_default_board));
    // in and out
    EXPECT_STREQ(cx_default_board, b.save_xfen().c_str());
    // default HASH
    EXPECT_EQ(ux_default_hash, b.hash());
}

TEST_F(ChessBoardTest, BoardCopyTest)
{
    chessboard b;
    EXPECT_EQ(e_none, b.load_xfen(cx_default_board));
    chessboard c;
    c.copy(b);
    // Note that XFEN goes through the engine it is not
    // simply a string attribute.
    EXPECT_STREQ(cx_default_board, c.save_xfen().c_str());
    EXPECT_EQ(ux_default_hash, c.hash());
    chessboard d = b;
    EXPECT_STREQ(cx_default_board, d.save_xfen().c_str());
    EXPECT_EQ(ux_default_hash, d.hash());
}

TEST_F(ChessBoardTest, BoardSaveLoadTest)
{
    // not purely loadable as a separate JSON file
    // check JSON object
    chessboard b;
    EXPECT_EQ(e_none, b.load_xfen(cx_default_board));
    auto jsonf = json::object();
    EXPECT_EQ(e_none, b.save(jsonf));
    EXPECT_EQ(true, jsonf.contains("xfen"));
    EXPECT_EQ(true, jsonf.contains("white_check"));
    EXPECT_EQ(true, jsonf.contains("black_check"));
    EXPECT_EQ(true, jsonf.contains("turn"));
    EXPECT_EQ(true, jsonf.contains("captured"));
    EXPECT_EQ(cx_default_board, jsonf["xfen"]);
    chessboard c;
    EXPECT_EQ(e_none, c.load(jsonf));
    EXPECT_EQ(cx_default_board, c.save_xfen());
}

TEST_F(ChessBoardTest, BoardPossibleMovesTest)
{
    chessboard b;
    EXPECT_EQ(e_none, b.load_xfen(cx_black_win));
    // Expected white possible moves - white lost and is in checkmate, so ....
    std::string exp_white = "";
    // 2nd flag is 'allow check' which we are passing as false
    // the AI angine eliminates checkmoves as they are considered
    // and the player is allowed to choose and the game will tell
    // them they are in check.  So this function exists for testing.
    std::vector<chessmove> v = b.possible_moves(c_white, false);
    EXPECT_EQ(exp_white, move_str_c4(v));
    // Expected black possible moves
    std::string exp_black = "g1f1 g1e1 g1d1 g1c1 g1b1 g1g2 g1h1 g1f2 g1e3 g1h2 g3g2 g3f3 g3g4 g3g5 g3g6 g3g7 g3g8 g3h3 b4b3 b4a4 "
                            "b4b5 b4b6 b4b7 b4b8 b4c4 b4d4 b4e4 b4f4 b4g4 b4h4 a5a4 e5e4 c6c5 d6c5 d6d5 d6e6 d6c7 d6d7 d6e7 f7f6 f7f5";
    v = b.possible_moves(c_black, false);
    // std::cout << move_str_c4(v);
    EXPECT_EQ(exp_black, move_str_c4(v));
}

TEST_F(ChessBoardTest, BoardXFENTest)
{
    // Check the famous wiki example first
    chessboard a;
    EXPECT_EQ(e_none, a.load_xfen(cx_wiki));
    EXPECT_EQ(cx_wiki, a.save_xfen());

    for (size_t i = 0; i < cx_puz_count; i++)
    {
        chessboard b;
        EXPECT_EQ(e_none, b.load_xfen(cx_puz[i]));
        EXPECT_EQ(cx_puz[i], b.save_xfen());
    }
}

TEST_F(ChessBoardTest, BoardCheckStateTest)
{
    chessboard b;
    // Default Board
    EXPECT_EQ(e_none, b.load_xfen(cx_default_board));
    // Neither should be in check
    EXPECT_EQ(false, b.check_state(c_white));
    EXPECT_EQ(false, b.check_state(c_black));

    // Black win board
    EXPECT_EQ(e_none, b.load_xfen(cx_black_win));
    // Check state - white is in CHECK and black is not
    EXPECT_EQ(true, b.check_state(c_white));
    EXPECT_EQ(false, b.check_state(c_black));

    // Black check board
    EXPECT_EQ(e_none, b.load_xfen(cx_black_check));
    // Check state - white is in CHECK and black is not
    EXPECT_EQ(false, b.check_state(c_white));
    EXPECT_EQ(true, b.check_state(c_black));

    // White check board
    EXPECT_EQ(e_none, b.load_xfen(cx_white_check));
    // Check state - white is in CHECK and black is not
    EXPECT_EQ(true, b.check_state(c_white));
    EXPECT_EQ(false, b.check_state(c_black));
}

TEST_F(ChessBoardTest, BoardTurnColorTest)
{
    chessboard b;
    // Default Board
    EXPECT_EQ(e_none, b.load_xfen(cx_default_board));
    // Check move
    EXPECT_EQ(c_white, b.turn_color());
    // Try valid
    chessmove m = b.attempt_move(c_white, "d2d4");
    EXPECT_EQ(e_none, m.error);
    EXPECT_EQ(c_black, b.turn_color());
    // Try invalid
    m = b.attempt_move(c_white, "d7d5");
    EXPECT_EQ(e_out_of_turn, m.error);
    m = b.attempt_move(c_black, "b2b4");
    EXPECT_EQ(e_invalid_move, m.error);
    // Try valid
    m = b.attempt_move(c_black, "dyd5");
    EXPECT_EQ(e_none, m.error);
    EXPECT_EQ(c_white, b.turn_color());
}

TEST_F(ChessBoardTest, BoardFindPieceTest)
{
    chessboard b;
    coord_s c;
    // Default Board
    EXPECT_EQ(e_none, b.load_xfen(cx_default_board));
    EXPECT_EQ(true, b.find_piece(p_king, c_white, c));
    EXPECT_EQ(true, coord_s(0, 4) == c);
    EXPECT_EQ(true, b.find_piece(p_king, c_black, c));
    EXPECT_EQ(true, coord_s(7, 4) == c);
    // White Check Board
    EXPECT_EQ(e_none, b.load_xfen(cx_white_check));
    EXPECT_EQ(true, b.find_piece(p_king, c_white, c));
    EXPECT_EQ(true, coord_s(0, 7) == c);
    EXPECT_EQ(true, b.find_piece(p_king, c_black, c));
    EXPECT_EQ(true, coord_s(7, 7) == c);
    EXPECT_EQ(true, b.find_piece(p_queen, c_black, c));
    EXPECT_EQ(true, coord_s(0, 6) == c);
    EXPECT_EQ(true, b.find_piece(p_knight, c_white, c));
    EXPECT_EQ(true, coord_s(2, 2) == c);
}

TEST_F(ChessBoardTest, BoardCapturedPieceTest)
{
    chessboard b;
    // Default Board
    EXPECT_EQ(e_none, b.load_xfen(cx_default_board));
    EXPECT_EQ("", b.captured_pieces_abbr(c_white));
    EXPECT_EQ("", b.captured_pieces_abbr(c_black));
    // This is kind of lame because an XFEN load
    // builds up a captured piece string based on rank as to
    // what is missing and does not assume any promotion or
    // promotion capture has occurred
    EXPECT_EQ(e_none, b.load_xfen(cx_white_check));
    EXPECT_EQ("PPPPPBNQ", b.captured_pieces_abbr(c_white));
    EXPECT_EQ("ppppbnnr", b.captured_pieces_abbr(c_black));
}