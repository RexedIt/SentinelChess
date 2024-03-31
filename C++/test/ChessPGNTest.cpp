#include "gtest/gtest.h"

#include "chesscommon.h"
#include "chesspgn.h"

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
    EXPECT_EQ(e_none, p.load(test_file("00000001.pgn")));
    EXPECT_EQ("Rated Classical game", p.event);
    EXPECT_EQ("BFG9k", p.white);
    EXPECT_EQ("mamalak", p.black);
    EXPECT_EQ(1639, p.whiteelo);
    EXPECT_EQ(1403, p.blackelo);
    EXPECT_EQ("C00", p.eco);
    EXPECT_EQ("French Defense: Normal Variation", p.opening);
    EXPECT_EQ("1-0", p["Result"]);
    EXPECT_EQ("2012.12.31", p["UTCDate"]);
    EXPECT_EQ("23:01:03", p["UTCTime"]);
    EXPECT_EQ("600+8", p["TimeControl"]);
    EXPECT_EQ("Normal", p["Termination"]);
    std::string expected_moves = "1. e4 e6 2. d4 b6 3. a3 Bb7 4. Nc3 Nh6 5. Bxh6 gxh6 6. Be2 Qg5 7. Bg4 h5 "
                                 "8. Nf3 Qg6 9. Nh4 Qg5 10. Bxh5 Qxh4 11. Qf3 Kd8 12. Qxf7 Nc6 13. Qe8# 1-0";
    EXPECT_EQ(expected_moves, p.moves);
}
