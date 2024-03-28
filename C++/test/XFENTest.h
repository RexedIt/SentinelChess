#pragma once

namespace chess
{
    const uint32_t ux_default_hash = 3597260180;
    const char *cx_default_board = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

    const char *cx_black_win = "8/5p2/2pk4/p3p2Q/1r6/1P2PKrP/P3P2R/RN4q1 w - - 0 67";
    const char *cx_black_check = "r1b2rk1/ppp4B/2nq1n2/3p2B1/2Pp4/2P1P3/P1Q2PP1/R3K1NR b KQ - 0 13";
    const char *cx_white_check = "5r1k/ppR5/6B1/6p1/3b4/2N5/PP3p1P/5RqK w - - 1 28";

    const char *cx_wiki = "rn2k1r1/ppp1pp1p/3p2p1/5bn1/P7/2N2B2/1PPPPP2/2BNK1RR w Qkq - 4 11";

    const size_t cx_puz_count = 25;
    const char *cx_puz[] = {
        "6nr/pp3p1p/k1p5/8/1QN5/2P1P3/4KPqP/8 b - - 5 26",
        "r1qr3k/pp3pB1/1np1pb2/8/3P3P/2N2PR1/PP1Q2P1/2KR4 b - - 0 22",
        "8/2q2rpk/p3R2p/1r6/4Q3/4P1P1/PP3PP1/6K1 b - - 4 38",
        "1kr5/p3q2r/Rpbpp3/2p2pp1/2PPn3/Q1P1P3/P5BP/KR4B1 b - - 9 31",
        "8/5kp1/7p/5p2/N3p3/pr5P/R4PP1/5K2 w - - 0 40",
        "r1b1kb1r/1p2np2/p1p4p/4PPp1/3PN3/8/PPP3PP/R1B2RK1 b kq - 0 15",
        "8/8/4p3/8/1k1p4/2p5/2K2P2/1N6 w - - 0 55",
        "r2qk2r/p2nb1pp/2p1p2B/3p4/8/2N5/PPP3PP/R2Q1RK1 b kq - 1 15",
        "8/8/p7/2P4p/1Pr1k3/7R/6K1/8 b - - 8 63",
        "r3k2r/ppqnbpp1/2p1pnp1/2Pp4/3P4/1QN1P3/PP2BPPP/R1B2RK1 w kq - 0 12",
        "r1b2k1r/pp1nqBpp/3B4/b5N1/5Pn1/2P5/P3Q1PP/RN2K2R b KQ - 0 15",
        "3r3k/Bp4p1/3p3p/4p2q/8/2N2r1P/PPP1QP1K/R4R2 w - - 0 24",
        "r4k2/pp3ppB/2p2p2/8/3P3N/2P1R1b1/PP3P1q/R2Q2K1 w - - 5 22",
        "5r2/4p1k1/3p4/p7/2Q2bPK/1r5P/8/3R1R2 w - - 2 34",
        "2Q5/2p2Rpk/3p3p/3P3P/8/1p6/2q3PK/4r3 b - - 2 39",
        "k3r3/p1p4p/3p1p2/3P1p2/2PP2n1/7Q/P4PPP/1R2q1K1 w - - 7 26",
        "r4rkR/2p1q1p1/p1p2pn1/3pPQN1/3P4/8/PP3P2/2KR4 b - - 1 22",
        "5rk1/1p5p/p5pQ/3pPb2/3P1N2/1P6/3q1R1P/6K1 b - - 3 27",
        "r4bk1/7p/1p3pp1/p7/2p1P3/RB1p3P/1P1K1PP1/7R b - - 0 28",
        "8/ppp4p/3p1p1R/1P1Pk3/4Pp2/3K1P2/P1P2Pr1/8 b - - 4 27",
        "6k1/pp3pR1/1q2b2R/3pP3/3P4/1r2Q2P/6P1/6K1 b - - 0 31",
        "q6k/1p1b1p2/1p2r2p/1Pp1p3/2P1P1Q1/3P2RP/5PP1/6K1 b - - 3 29",
        "k3r3/p1p4p/3p1p2/3P1p2/2PP2n1/7Q/P4PPP/1R2q1K1 w - - 7 26",
        "r1b2b1r/pp1n1kp1/2p3p1/Q2np1B1/8/8/PPP2PPP/RN2K2R w KQ - 2 11",
        "2r5/7p/6p1/5k2/5P2/2RKP3/4P2P/8 b - - 0 34",
    };
}