#include "gtest/gtest.h"

#include "chesscommon.h"
#include "chessboard.h"

using namespace chess;
using namespace testing;

const char *c_default_board = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

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
    b.load_xfen(c_default_board);
    // in and out
    EXPECT_STREQ(c_default_board, b.save_xfen().c_str());
    // default HASH
    uint32_t expected_hash = 3310897876;
    EXPECT_EQ(expected_hash, b.hash());
    // Same with empty captured pieces
    EXPECT_EQ("", b.captured_pieces_abbr(c_white));
    EXPECT_EQ("", b.captured_pieces_abbr(c_black));
}

TEST_F(ChessBoardTest, BoardCopyTest)
{
    chessboard b;
    b.load_xfen(c_default_board);
    uint32_t expected_hash = 3310897876;
    chessboard c;
    c.copy(b);
    EXPECT_STREQ(c_default_board, c.save_xfen().c_str());
    EXPECT_EQ(expected_hash, c.hash());
    chessboard d = b;
    EXPECT_STREQ(c_default_board, d.save_xfen().c_str());
    EXPECT_EQ(expected_hash, d.hash());
}

// TEST_F(ChessBoardTest, BoardSaveTest) {;}
// TEST_F(ChessBoardTest, BoardLoadTest) {;}
// TEST_F(ChessBoardTest, BoardPossibleMovesTest) {;}
// TEST_F(ChessBoardTest, BoardCheckStateTest) {;}
// TEST_F(ChessBoardTest, BoardTurnColorTest) {;}
// TEST_F(ChessBoardTest, BoardFindCheckTest) {;}
// TEST_F(ChessBoardTest, BoardFindPieceTest) {;}