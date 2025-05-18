#include "tiles.h"
#include "winning_hand.h"
#include <cassert>
#include <iostream>

void resetGame(){
    Tiles aka; 
    initGameStatus("1m", aka, "1z", "2z");
}

void compound_pinfu_tanyao(){
    resetGame();
    WinningHand h{};
    h.tileIds = {m3,m6,p4,s5,0,0,0};
    h.groupTypes = {1,1,1,1};
    h.pairTileId = m8;
    h.waitMeldIndex = 0;
    h.waitTileId = m5;
    h.calculateHan();
    assert(h.totalHan == 2);
}

void compound_toitoi_winds(){
    resetGame();
    WinningHand h{};
    h.tileIds = {m3,p5,s7,z1,0,0,0};
    h.groupTypes = {0,0,0,0};
    h.pairTileId = m9;
    h.waitMeldIndex = 3;
    h.waitTileId = z1;
    h.calculateHan();
    assert(h.totalHan == 5);
}

void compound_iipeikou_pinfu(){
    resetGame();
    WinningHand h{};
    h.tileIds = {m3,m3,m6,p5,0,0,0};
    h.groupTypes = {1,1,1,1};
    h.pairTileId = m1;
    h.waitMeldIndex = 0;
    h.waitTileId = m5;
    h.calculateHan();
    assert(h.totalHan == 2);
}

void compound_honitsu_toitoi(){
    resetGame();
    WinningHand h{};
    h.tileIds = {m1,m4,m7,z5,0,0,0};
    h.groupTypes = {0,0,0,0};
    h.pairTileId = m9;
    h.waitMeldIndex = 0;
    h.waitTileId = m1;
    h.calculateHan();
    assert(h.totalHan == 7);
}

void compound_ryanpeikou(){
    resetGame();
    WinningHand h{};
    h.tileIds = {m3,m3,p5,p5,0,0,0};
    h.groupTypes = {1,1,1,1};
    h.pairTileId = m1;
    h.waitMeldIndex = 0;
    h.waitTileId = m4;
    h.calculateHan();
    assert(h.totalHan == 3);
}

void compound_ittsuu(){
    resetGame();
    WinningHand h{};
    h.tileIds = {m1,m4,m7,p3,0,0,0};
    h.groupTypes = {1,1,1,1};
    h.pairTileId = z1;
    h.waitMeldIndex = 0;
    h.waitTileId = m3;
    h.calculateHan();
    assert(h.totalHan == 3);
}

void compound_sanshoku(){
    resetGame();
    WinningHand h{};
    h.tileIds = {m3,m7,p3,s3,0,0,0};
    h.groupTypes = {1,0,1,1};
    h.pairTileId = z1;
    h.waitMeldIndex = 0;
    h.waitTileId = m5;
    h.calculateHan();
    assert(h.totalHan == 2);
}

void compound_chiitoitsu(){
    resetGame();
    WinningHand h{};
    h.isChiitoitsu = true;
    h.tileIds = {m1,m2,m3,m4,p5,p6,p7};
    h.waitTileId = -1;
    h.calculateHan();
    assert(h.totalHan == 4);
}

void compound_sanrenkou(){
    resetGame();
    WinningHand h{};
    h.tileIds = {m3,m4,m5,m7,0,0,0};
    h.groupTypes = {0,0,0,0};
    h.pairTileId = z1;
    h.waitMeldIndex = 0;
    h.waitTileId = m3;
    h.calculateHan();
    assert(h.totalHan == 10);
}

void compound_kokuiisou(){
    resetGame();
    WinningHand h{};
    h.isChiitoitsu = true;
    h.tileIds = {p2,p4,p8,z1,z2,z3,z4};
    h.calculateHan();
    assert(h.totalHan == 10000);
}

int main(){
    compound_pinfu_tanyao();
    compound_toitoi_winds();
    compound_iipeikou_pinfu();
    compound_honitsu_toitoi();
    compound_ryanpeikou();
    compound_ittsuu();
    compound_sanshoku();
    compound_chiitoitsu();
    compound_sanrenkou();
    compound_kokuiisou();
    std::cout << "Compound tests passed\n";
    return 0;
}
