#include "gtest/gtest.h"

#include "chesscommon.h"
#include "chessopenings.h"
#include "chesslobby.h"
#include "ECOTest.h"

using namespace chess;
using namespace testing;

extern std::string test_datapath;

class ChessOpeningTest : public Test
{
public:
    ChessOpeningTest() { ; }
    ~ChessOpeningTest() { ; }

    std::string test_file(std::string orig)
    {
        return fix_path(test_datapath + "\\PGN\\" + orig);
    }

protected:
    void SetUp() override { ; }
    // Sets up the test fixture.
    void TearDown() override { ; }
};

TEST_F(ChessOpeningTest, LoadTest)
{
    chessopening co;
    std::string errextra;
    EXPECT_EQ(e_none, load_scid_line(cx_eco[cx_eco_typical_1], co, errextra));
    EXPECT_EQ("", errextra);
    EXPECT_EQ("A00", co.eco);
    EXPECT_EQ("Barnes Opening", co.title);
    EXPECT_EQ(1, co.moves.size());
    EXPECT_EQ(e_none, load_scid_line(cx_eco[cx_eco_typical_2], co, errextra));
    EXPECT_EQ("", errextra);
    EXPECT_EQ("C63", co.eco);
    EXPECT_EQ("Spanish: Schliemann, 4.Nc3 fxe4 5.Nxe4 d5", co.title);
    EXPECT_EQ(10, co.moves.size());
}

TEST_F(ChessOpeningTest, SequenceAnomaly)
{
    chessopening co;
    std::string errextra;
    //"A05 \"Reti: KIA, Reti-Smyslov Variation\"  1.Nf3 Nf6 2.g3 g6 3.b4 Bg7 5.Bb2 *",
    EXPECT_EQ(e_loading, load_scid_line(cx_eco[cx_eco_seqerr_1], co, errextra));
    EXPECT_EQ("Full move sequence error at 4.", errextra);
    //"C40m \"Latvian Gambit: 3.d4 fxe4 5.Nxe5 Nf6\" 1.e4 e5 2.Nf3 f5 3.d4 fxe4 5.Nxe5 Nf6 *",
    EXPECT_EQ(e_loading, load_scid_line(cx_eco[cx_eco_seqerr_2], co, errextra));
    EXPECT_EQ("Full move sequence error at 4.", errextra);
    //"D20f \"QGA: 3.e3 e5\" 1.d4 d5 2.c4 dxc4 3.e3 e5 4.Bxc4 exd4 5.exd4 Bb4+ 5.Nc3 Nf6 *",
    EXPECT_EQ(e_loading, load_scid_line(cx_eco[cx_eco_seqerr_3], co, errextra));
    EXPECT_EQ("Full move sequence error at 6.", errextra);
}

TEST_F(ChessOpeningTest, MalformedLine)
{
    chessopening co;
    std::string errextra;
    //"E09a \"\"Catalan: Closed, Main Line\" 1.de4 Nf6 2.c4 e6 3.g3 d5 4.Kg2 Be7 5.Nf32 O-O 6.O-O Nbd7 7.Qc2 c6 8.Nbd2 *"};
    EXPECT_EQ(e_invalid_move, load_scid_line(cx_eco[cx_eco_malformed], co, errextra));
    EXPECT_EQ("Move parse error - de4 at 2.", errextra);
}

TEST_F(ChessOpeningTest, RecognitionPGN2)
{
    chesslobby l;
    std::string errextra;
    EXPECT_EQ(e_none, l.load_game(test_file("00000002.pgn"), errextra));
    EXPECT_EQ("", errextra);
    // Note, the file claims ECO variation "Queen's Pawn Game: Colle System, Anti-Colle"
    // Which may match our actual ECO, although our title from the scid is "Colle: 3...Bf5, Alekhine Variation"
    // Could be the same and this match in the scid represents the closest moves to actual
    // moves taken.
    EXPECT_EQ("D04", l.game()->eco());
    EXPECT_EQ("Colle: 3...Bf5, Alekhine Variation", l.game()->open_title());
}

TEST_F(ChessOpeningTest, RecognitionPGN3)
{
    chesslobby l;
    std::string errextra;
    EXPECT_EQ(e_none, l.load_game(test_file("00000003.pgn"), errextra));
    EXPECT_EQ("", errextra);
    // Note, the file claims ECO C50 and Four Knights Game: Italian Variation
    // The actual closest turn sequence in the file is C55, Two Knights: Ulvestad Variation
    // As per the actual turns taken so this is what our system returns.
    EXPECT_EQ("C55", l.game()->eco());
    EXPECT_EQ("Two Knights: Ulvestad Variation", l.game()->open_title());
}
