#include "tiles.h"
#include "winning_hand.h"
#include <cassert>
#include <iostream>

int main() {
    // game setup without red fives or dora for simplicity
    Tiles aka;
    initGameStatus("1m", aka, "1z", "2z");

    // Pinfu: sequences only, non-value pair, two sided wait on 2m for 2-3-4
    WinningHand pinfu;
    pinfu.tileIds = {m2, p2, s2, m5, 0, 0, 0};
    pinfu.groupTypes = {1,1,1,1};
    pinfu.pairTileId = m8;
    pinfu.waitMeldIndex = 0;
    pinfu.waitTileId = m2;
    assert(pinfu.pinfu().han == 1);

    // Tanyao: no honors or terminals
    WinningHand tanyao = pinfu;
    tanyao.waitTileId = m5;
    assert(tanyao.tanyao().han == 1);

    // Chiitoitsu
    WinningHand chii;
    chii.isChiitoitsu = true;
    chii.tileIds = {m1,m2,m3,m4,m5,m6,m7};
    assert(chii.chitoitsu().han == 2);

    // Toitoi: all triplets
    WinningHand toitoi;
    toitoi.tileIds = {m1,m2,m3,m4,0,0,0};
    toitoi.groupTypes = {0,0,0,0};
    toitoi.pairTileId = m5;
    assert(toitoi.toitoi().han == 2);

    // Sanshoku doujun
    WinningHand sanshoku;
    sanshoku.tileIds = {m2,p2,s2,m5,0,0,0};
    sanshoku.groupTypes = {1,1,1,1};
    sanshoku.pairTileId = z1;
    assert(sanshoku.sanshoku().han == 2);

    // Ittsuu
    WinningHand ittsuu;
    ittsuu.tileIds = {m1,m4,m7,p2,0,0,0};
    ittsuu.groupTypes = {1,1,1,0};
    ittsuu.pairTileId = m2;
    assert(ittsuu.ittsuu().han == 2);

    // Sanankou: three concealed triplets
    WinningHand sanankou;
    sanankou.tileIds = {m1,m2,m3,m4,0,0,0};
    sanankou.groupTypes = {0,0,0,0};
    sanankou.pairTileId = m5;
    sanankou.waitMeldIndex = -1;
    assert(sanankou.sanankou().han == 2);

    // Shousangen
    WinningHand shousangen;
    shousangen.tileIds = {z5,z6,m1,m2,0,0,0};
    shousangen.groupTypes = {0,0,0,0};
    shousangen.pairTileId = z7;
    assert(shousangen.shousangen().han == 2);

    // Daisangen
    WinningHand daisangen;
    daisangen.tileIds = {z5,z6,z7,m1,0,0,0};
    daisangen.groupTypes = {0,0,0,0};
    daisangen.pairTileId = m2;
    assert(daisangen.daisangen().han == 10000);

    // Dai suushi
    WinningHand suushi;
    suushi.tileIds = {z1,z2,z3,z4,0,0,0};
    suushi.groupTypes = {0,0,0,0};
    suushi.pairTileId = m1;
    assert(suushi.suushi().han == 20000);

    // Tsuuiisou
    WinningHand tsuui;
    tsuui.tileIds = {z1,z2,z5,z6,0,0,0};
    tsuui.groupTypes = {0,0,0,0};
    tsuui.pairTileId = z3;
    assert(tsuui.tsuuiisou().han == 10000);

    // Ryuuiisou
    WinningHand ryuu;
    ryuu.tileIds = {s2,s3,s4,z6,0,0,0};
    ryuu.groupTypes = {0,0,0,0};
    ryuu.pairTileId = s8;
    assert(ryuu.ryuuiisou().han == 10000);

    // Chinroutou
    WinningHand chinrou;
    chinrou.tileIds = {m1,m9,p1,s9,0,0,0};
    chinrou.groupTypes = {0,0,0,0};
    chinrou.pairTileId = m1;
    assert(chinrou.chinroutou().han == 10000);

    // Suuankou tanki
    WinningHand suuankou;
    suuankou.tileIds = {m1,m2,m3,m4,0,0,0};
    suuankou.groupTypes = {0,0,0,0};
    suuankou.pairTileId = m5;
    suuankou.waitMeldIndex = 4; // pair wait
    suuankou.waitTileId = m5;
    assert(suuankou.suuankouTanki().han == 20000);

    std::cout << "Yaku tests passed\n";
    return 0;
}
