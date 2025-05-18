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
    // Not pinfu if a triplet exists
    WinningHand pinfu_bad = pinfu;
    pinfu_bad.groupTypes[0] = 0;
    assert(pinfu_bad.pinfu().han == 0);

    // Tanyao: no honors or terminals
    WinningHand tanyao = pinfu;
    tanyao.waitTileId = m5;
    assert(tanyao.tanyao().han == 1);
    // Tanyao fails when containing a terminal
    WinningHand tanyao_bad = pinfu;
    tanyao_bad.tileIds[0] = m1;
    assert(tanyao_bad.tanyao().han == 0);

    // Chiitoitsu
    WinningHand chii;
    chii.isChiitoitsu = true;
    chii.tileIds = {m1,m2,m3,m4,m5,m6,m7};
    assert(chii.chitoitsu().han == 2);
    // Flag must be set for chiitoitsu
    WinningHand chii_bad = chii;
    chii_bad.isChiitoitsu = false;
    assert(chii_bad.chitoitsu().han == 0);

    // Toitoi: all triplets
    WinningHand toitoi;
    toitoi.tileIds = {m1,m2,m3,m4,0,0,0};
    toitoi.groupTypes = {0,0,0,0};
    toitoi.pairTileId = m5;
    assert(toitoi.toitoi().han == 2);
    // Sequence breaks toitoi
    WinningHand toitoi_bad = toitoi;
    toitoi_bad.groupTypes[1] = 1;
    assert(toitoi_bad.toitoi().han == 0);

    // Sanshoku doujun
    WinningHand sanshoku;
    sanshoku.tileIds = {m2,p2,s2,m5,0,0,0};
    sanshoku.groupTypes = {1,1,1,1};
    sanshoku.pairTileId = z1;
    assert(sanshoku.sanshoku().han == 2);
    // Triplet version also counts
    WinningHand sanshoku_triplet;
    sanshoku_triplet.tileIds = {m3,p3,s3,m7,0,0,0};
    sanshoku_triplet.groupTypes = {0,0,0,0};
    sanshoku_triplet.pairTileId = z2;
    assert(sanshoku_triplet.sanshoku().han == 2);

    // Ittsuu
    WinningHand ittsuu;
    ittsuu.tileIds = {m1,m4,m7,p2,0,0,0};
    ittsuu.groupTypes = {1,1,1,0};
    ittsuu.pairTileId = m2;
    assert(ittsuu.ittsuu().han == 2);
    // Missing a sequence cancels ittsuu
    WinningHand ittsuu_bad = ittsuu;
    ittsuu_bad.groupTypes[2] = 0;
    assert(ittsuu_bad.ittsuu().han == 0);

    // Sanankou: three concealed triplets
    WinningHand sanankou;
    sanankou.tileIds = {m1,m2,m3,m4,0,0,0};
    sanankou.groupTypes = {0,0,0,0};
    sanankou.pairTileId = m5;
    sanankou.waitMeldIndex = -1;
    assert(sanankou.sanankou().han == 2);
    // Only two concealed triplets is not enough
    WinningHand sanankou_bad = sanankou;
    sanankou_bad.groupTypes[2] = 1;
    sanankou_bad.waitMeldIndex = 0;
    assert(sanankou_bad.sanankou().han == 0);

    // Shousangen
    WinningHand shousangen;
    shousangen.tileIds = {z5,z6,m1,m2,0,0,0};
    shousangen.groupTypes = {0,0,0,0};
    shousangen.pairTileId = z7;
    assert(shousangen.shousangen().han == 2);
    // Missing dragon pair should fail
    WinningHand shousangen_bad = shousangen;
    shousangen_bad.pairTileId = m2;
    assert(shousangen_bad.shousangen().han == 0);

    // Daisangen
    WinningHand daisangen;
    daisangen.tileIds = {z5,z6,z7,m1,0,0,0};
    daisangen.groupTypes = {0,0,0,0};
    daisangen.pairTileId = m2;
    assert(daisangen.daisangen().han == 10000);
    // Only two dragon triplets gives nothing
    WinningHand daisangen_bad = daisangen;
    daisangen_bad.tileIds[2] = m1;
    assert(daisangen_bad.daisangen().han == 0);

    // Dai suushi
    WinningHand suushi;
    suushi.tileIds = {z1,z2,z3,z4,0,0,0};
    suushi.groupTypes = {0,0,0,0};
    suushi.pairTileId = m1;
    assert(suushi.suushi().han == 20000);
    // Ten winds is neither small nor big four winds
    WinningHand suushi_bad = suushi;
    suushi_bad.tileIds[3] = m1;
    assert(suushi_bad.suushi().han == 0);

    // Tsuuiisou
    WinningHand tsuui;
    tsuui.tileIds = {z1,z2,z5,z6,0,0,0};
    tsuui.groupTypes = {0,0,0,0};
    tsuui.pairTileId = z3;
    assert(tsuui.tsuuiisou().han == 10000);
    // Honors only yaku fails with suited tile
    WinningHand tsuui_bad = tsuui;
    tsuui_bad.tileIds[0] = m1;
    assert(tsuui_bad.tsuuiisou().han == 0);

    // Ryuuiisou
    WinningHand ryuu;
    ryuu.tileIds = {s2,s3,s4,z6,0,0,0};
    ryuu.groupTypes = {0,0,0,0};
    ryuu.pairTileId = s8;
    assert(ryuu.ryuuiisou().han == 10000);
    // Any non green tile cancels ryuuiisou
    WinningHand ryuu_bad = ryuu;
    ryuu_bad.pairTileId = s1;
    assert(ryuu_bad.ryuuiisou().han == 0);

    // Chinroutou
    WinningHand chinrou;
    chinrou.tileIds = {m1,m9,p1,s9,0,0,0};
    chinrou.groupTypes = {0,0,0,0};
    chinrou.pairTileId = m1;
    assert(chinrou.chinroutou().han == 10000);
    // Adding a simple tile breaks chinroutou
    WinningHand chinrou_bad = chinrou;
    chinrou_bad.tileIds[3] = s2;
    assert(chinrou_bad.chinroutou().han == 0);

    // Suuankou tanki
    WinningHand suuankou;
    suuankou.tileIds = {m1,m2,m3,m4,0,0,0};
    suuankou.groupTypes = {0,0,0,0};
    suuankou.pairTileId = m5;
    suuankou.waitMeldIndex = 4; // pair wait
    suuankou.waitTileId = m5;
    assert(suuankou.suuankouTanki().han == 20000);
    // Not waiting on pair means no yakuman
    WinningHand suuankou_bad = suuankou;
    suuankou_bad.waitMeldIndex = 0;
    suuankou_bad.waitTileId = m1;
    assert(suuankou_bad.suuankouTanki().han == 0);

    std::cout << "Yaku tests passed\n";
    return 0;
}
