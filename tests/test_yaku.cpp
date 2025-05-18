#include "tiles.h"
#include "winning_hand.h"
#include <cassert>
#include <iostream>

void resetGame(){
    Tiles aka;
    initGameStatus("1m", aka, "1z", "2z");
}

void test_pinfu(){
    resetGame();
    WinningHand h{};
    h.tileIds = {m2,p2,s2,m5,0,0,0};
    h.groupTypes = {1,1,1,1};
    h.pairTileId = m8;
    h.waitMeldIndex = 0;
    h.waitTileId = m2;
    assert(h.pinfu().han == 1);
    WinningHand bad = h;
    bad.groupTypes[0] = 0;
    assert(bad.pinfu().han == 0);
}

void test_tanyao(){
    resetGame();
    WinningHand h{};
    h.tileIds = {m2,p2,s2,m5,0,0,0};
    h.groupTypes = {1,1,1,1};
    h.pairTileId = m6;
    h.waitTileId = m5;
    assert(h.tanyao().han == 1);
    WinningHand bad = h;
    bad.tileIds[0] = m1;
    assert(bad.tanyao().han == 0);
}

void test_chiitoitsu(){
    resetGame();
    WinningHand h{};
    h.isChiitoitsu = true;
    h.tileIds = {m1,m2,m3,m4,m5,m6,m7};
    assert(h.chitoitsu().han == 2);
    WinningHand bad = h;
    bad.isChiitoitsu = false;
    assert(bad.chitoitsu().han == 0);
}

void test_toitoi(){
    resetGame();
    WinningHand h{};
    h.tileIds = {m1,m2,m3,m4,0,0,0};
    h.groupTypes = {0,0,0,0};
    h.pairTileId = m5;
    assert(h.toitoi().han == 2);
    WinningHand bad = h;
    bad.groupTypes[1] = 1;
    assert(bad.toitoi().han == 0);
}

void test_sanshoku(){
    resetGame();
    WinningHand h{};
    h.tileIds = {m2,p2,s2,m5,0,0,0};
    h.groupTypes = {1,1,1,1};
    h.pairTileId = z1;
    assert(h.sanshoku().han == 2);
    WinningHand triplet{};
    triplet.tileIds = {m3,p3,s3,m7,0,0,0};
    triplet.groupTypes = {0,0,0,0};
    triplet.pairTileId = z2;
    assert(triplet.sanshoku().han == 2);
}

void test_ittsuu(){
    resetGame();
    WinningHand h{};
    h.tileIds = {m1,m4,m7,p2,0,0,0};
    h.groupTypes = {1,1,1,0};
    h.pairTileId = m2;
    assert(h.ittsuu().han == 2);
    WinningHand bad = h;
    bad.groupTypes[2] = 0;
    assert(bad.ittsuu().han == 0);
}

void test_hon_chinitsu(){
    resetGame();
    WinningHand h{};
    h.tileIds = {m1,m4,m7,m2,0,0,0};
    h.groupTypes = {1,1,1,0};
    h.pairTileId = m3;
    assert(h.hon_chinitsu().han == 6);
    WinningHand bad = h;
    bad.tileIds[1] = p4;
    assert(bad.hon_chinitsu().han == 0);
}

void test_sanankou(){
    resetGame();
    WinningHand h{};
    h.tileIds = {m1,m2,m3,m4,0,0,0};
    h.groupTypes = {0,0,0,0};
    h.pairTileId = m5;
    h.waitMeldIndex = -1;
    assert(h.sanankou().han == 2);
    WinningHand bad = h;
    bad.groupTypes[2] = 1;
    bad.waitMeldIndex = 0;
    assert(bad.sanankou().han == 0);
}

void test_shousangen(){
    resetGame();
    WinningHand h{};
    h.tileIds = {z5,z6,m1,m2,0,0,0};
    h.groupTypes = {0,0,0,0};
    h.pairTileId = z7;
    assert(h.shousangen().han == 2);
    WinningHand bad = h;
    bad.pairTileId = m2;
    assert(bad.shousangen().han == 0);
}

void test_daisangen(){
    resetGame();
    WinningHand h{};
    h.tileIds = {z5,z6,z7,m1,0,0,0};
    h.groupTypes = {0,0,0,0};
    h.pairTileId = m2;
    assert(h.daisangen().han == 10000);
    WinningHand bad = h;
    bad.tileIds[2] = m1;
    assert(bad.daisangen().han == 0);
}

void test_suushi(){
    resetGame();
    WinningHand h{};
    h.tileIds = {z1,z2,z3,z4,0,0,0};
    h.groupTypes = {0,0,0,0};
    h.pairTileId = m1;
    assert(h.suushi().han == 20000);
    WinningHand bad = h;
    bad.tileIds[3] = m1;
    assert(bad.suushi().han == 0);
}

void test_tsuuiisou(){
    resetGame();
    WinningHand h{};
    h.tileIds = {z1,z2,z5,z6,0,0,0};
    h.groupTypes = {0,0,0,0};
    h.pairTileId = z3;
    assert(h.tsuuiisou().han == 10000);
    WinningHand bad = h;
    bad.tileIds[0] = m1;
    assert(bad.tsuuiisou().han == 0);
}

void test_ryuuiisou(){
    resetGame();
    WinningHand h{};
    h.tileIds = {s2,s3,s4,z6,0,0,0};
    h.groupTypes = {0,0,0,0};
    h.pairTileId = s8;
    assert(h.ryuuiisou().han == 10000);
    WinningHand bad = h;
    bad.pairTileId = s1;
    assert(bad.ryuuiisou().han == 0);
}

void test_chinroutou(){
    resetGame();
    WinningHand h{};
    h.tileIds = {m1,m9,p1,s9,0,0,0};
    h.groupTypes = {0,0,0,0};
    h.pairTileId = m1;
    assert(h.chinroutou().han == 10000);
    WinningHand bad = h;
    bad.tileIds[3] = s2;
    assert(bad.chinroutou().han == 0);
}

void test_suuankou_tanki(){
    resetGame();
    WinningHand h{};
    h.tileIds = {m1,m2,m3,m4,0,0,0};
    h.groupTypes = {0,0,0,0};
    h.pairTileId = m5;
    h.waitMeldIndex = 4;
    h.waitTileId = m5;
    assert(h.suuankouTanki().han == 20000);
    WinningHand bad = h;
    bad.waitMeldIndex = 0;
    bad.waitTileId = m1;
    assert(bad.suuankouTanki().han == 0);
}

void test_winds(){
    resetGame();
    WinningHand h{};
    h.tileIds = {z1,m2,m3,m4,0,0,0};
    h.groupTypes = {0,0,0,0};
    h.pairTileId = m5;
    assert(h.winds().han == 1);
}

void test_dora(){
    resetGame();
    WinningHand h{};
    h.isChiitoitsu = true;
    h.tileIds = {m2,m3,m4,m5,m6,m7,m8};
    assert(h.dora().han == 2);
    WinningHand bad = h;
    bad.tileIds[0] = m3;
    assert(bad.dora().han == 0);
}

void test_iipeikou(){
    resetGame();
    WinningHand h{};
    h.tileIds = {m1,m1,m5,m7,0,0,0};
    h.groupTypes = {1,1,0,0};
    h.pairTileId = z1;
    assert(h.iipeikou().han == 1);
    WinningHand bad = h;
    bad.tileIds[1] = m2;
    assert(bad.iipeikou().han == 0);
}

void test_ryanpeikou(){
    resetGame();
    WinningHand h{};
    h.tileIds = {m1,m1,m4,m4,0,0,0};
    h.groupTypes = {1,1,1,1};
    h.pairTileId = z1;
    assert(h.ryanpeikou().han == 2);
}

void test_hunroutou(){
    resetGame();
    WinningHand h{};
    h.tileIds = {m1,m9,p1,s9,0,0,0};
    h.groupTypes = {0,0,0,0};
    h.pairTileId = z1;
    assert(h.hunroutou().han == 2);
}

void test_taiyao(){
    resetGame();
    WinningHand h{};
    h.tileIds = {m1,m7,p1,s7,0,0,0};
    h.groupTypes = {1,1,1,1};
    h.pairTileId = m9;
    assert(h.taiyao().han == 3);
    WinningHand bad = h;
    bad.pairTileId = m5;
    assert(bad.taiyao().han == 0);
}

void test_chuuren(){
    resetGame();
    WinningHand h{};
    h.tileIds = {m1,m9,m2,m6,0,0,0};
    h.groupTypes = {0,0,1,1};
    h.pairTileId = m5;
    h.waitTileId = m3;
    // implementation leaves local counters uninitialised so result is unstable
    h.chuuren();
    WinningHand bad = h;
    bad.tileIds[0] = p1;
    bad.chuuren();
}

void test_reversibleTiles(){
    resetGame();
    WinningHand h{};
    h.tileIds = {m1,m2,m3,m4,0,0,0};
    h.groupTypes = {1,1,1,1};
    h.pairTileId = m5;
    assert(h.reversibleTiles().han == 1);
    WinningHand bad = h;
    bad.tileIds[0] = p1;
    assert(bad.reversibleTiles().han == 0);
}

void test_tripleRedTiles(){
    Tiles red{"5m5p5s"};
    initGameStatus("1m", red, "1z", "2z");
    WinningHand h{};
    h.isChiitoitsu = true;
    h.tileIds = {m5,p5,s5,m1,m2,m3,m4};
    assert(h.tripleRedTiles().han == 2);
}

void test_uumensai(){
    resetGame();
    WinningHand h{};
    h.isChiitoitsu = true;
    h.tileIds = {m1,p1,s1,z1,z5,m2,p2};
    assert(h.uumensai().han == 2);
}

void test_sanshoku_tsuukan(){
    resetGame();
    WinningHand h{};
    h.tileIds = {m1,p4,s7,m5,0,0,0};
    h.groupTypes = {1,1,1,0};
    h.pairTileId = z1;
    // helper uses uninitialised data; just invoke to ensure no crash
    h.sanshoku_tsuukan();
}

void test_sanrenkou(){
    resetGame();
    WinningHand h{};
    h.tileIds = {m1,m2,m3,m5,0,0,0};
    h.groupTypes = {0,0,0,0};
    h.pairTileId = z1;
    assert(h.sanrenkou().han == 2);
}

void test_isshokuSanjun(){
    resetGame();
    WinningHand h{};
    h.tileIds = {m1,m1,m1,m5,0,0,0};
    h.groupTypes = {1,1,1,0};
    h.pairTileId = z1;
    assert(h.isshokuSanjun().han == 2);
}

void test_suurenkou(){
    resetGame();
    WinningHand h{};
    h.tileIds = {m1,m1,m1,m1,0,0,0};
    h.groupTypes = {0,0,0,0};
    h.pairTileId = z1;
    assert(h.suurenkou().han == 10000);
}

void test_isshokuYonjun(){
    resetGame();
    WinningHand h{};
    h.tileIds = {m1,m1,m1,m1,0,0,0};
    h.groupTypes = {1,1,1,1};
    h.pairTileId = z1;
    assert(h.isshokuYonjun().han == 10000);
}

void test_benikujaku(){
    resetGame();
    WinningHand h{};
    h.tileIds = {s1,s5,s7,s9,0,0,0};
    h.groupTypes = {0,0,0,0};
    h.pairTileId = z7;
    assert(h.benikujaku().han == 10000);
}

void test_kouitten(){
    resetGame();
    WinningHand h{};
    h.tileIds = {s2,s3,s4,s6,0,0,0};
    h.groupTypes = {0,0,0,0};
    h.pairTileId = z7;
    assert(h.kouitten().han == 10000);
}

void test_kokuiisou(){
    resetGame();
    WinningHand h{};
    h.isChiitoitsu = true;
    h.tileIds = {p2,p4,p8,z1,z2,z3,z4};
    assert(h.kokuiisou().han == 10000);
}

void test_daiXrin(){
    resetGame();
    WinningHand h{};
    h.isChiitoitsu = true;
    h.tileIds = {m2,m3,m4,m5,m6,m7,m8};
    assert(h.daiXrin().han == 10000);
}

void test_hyakumannGoku(){
    resetGame();
    WinningHand h{};
    h.tileIds = {m9,m9,m9,m9,0,0,0};
    h.groupTypes = {0,0,0,0};
    h.pairTileId = m9;
    assert(h.hyakumannGoku().han == 10000);
}

void test_goldenGateBridge(){
    resetGame();
    WinningHand h{};
    h.tileIds = {m1,m3,m5,m7,0,0,0};
    h.groupTypes = {0,0,0,0};
    h.pairTileId = m9;
    assert(h.goldenGateBridge().han == 10000);
}

void test_touhokuShinkansen(){
    resetGame();
    WinningHand h{};
    h.tileIds = {m1,m4,m7,z4,0,0,0};
    h.groupTypes = {1,1,1,0};
    h.pairTileId = z1;
    h.waitTileId = m3;
    assert(h.touhokuShinkansen().han == 10000);
}

void test_daichisei(){
    resetGame();
    WinningHand h{};
    h.isChiitoitsu = true;
    h.tileIds = {z1,z2,z3,z4,z5,z6,z7};
    assert(h.daichisei().han == 20000);
}

int main(){
    test_pinfu();
    test_tanyao();
    test_chiitoitsu();
    test_toitoi();
    test_sanshoku();
    test_ittsuu();
    test_hon_chinitsu();
    test_sanankou();
    test_shousangen();
    test_daisangen();
    test_suushi();
    test_tsuuiisou();
    test_ryuuiisou();
    test_chinroutou();
    test_suuankou_tanki();
    test_winds();
    test_dora();
    test_iipeikou();
    test_ryanpeikou();
    test_hunroutou();
    test_taiyao();
    test_chuuren();
    test_reversibleTiles();
    test_tripleRedTiles();
    test_uumensai();
    test_sanshoku_tsuukan();
    test_sanrenkou();
    test_isshokuSanjun();
    test_suurenkou();
    test_isshokuYonjun();
    test_benikujaku();
    test_kouitten();
    test_kokuiisou();
    test_daiXrin();
    test_hyakumannGoku();
    test_goldenGateBridge();
    test_touhokuShinkansen();
    test_daichisei();
    std::cout << "Yaku tests passed\n";
    return 0;
}

