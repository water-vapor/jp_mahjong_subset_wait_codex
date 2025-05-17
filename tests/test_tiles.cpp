#include "tiles.h"
#include "winning_hand.h"
#include <cassert>
#include <iostream>

int main(){
    assert(Tiles::tileIndex("1m") == 0);
    assert(Tiles::tileIndex("9s") == 26);
    assert(Tiles::isYaochu(m1));
    assert(!Tiles::isYaochu(p2));

    // test initGameStatus with multiple red fives
    Tiles aka{"5m5p"};
    initGameStatus("1m", aka, "1z", "2z");
    assert(gameInfo.red5m && gameInfo.red5p && !gameInfo.red5s);

    // simple pinfu check: waiting on 3m to complete 3-4-5
    WinningHand hand;
    hand.tileIds = {m3, m4, p2, s5, 0, 0, 0}; // only first four used
    hand.groupTypes = {1, 1, 1, 1};
    hand.pairTileId = z3; // non-value pair
    hand.waitMeldIndex = 0;
    hand.waitTileId = m3;
    hand.pinfu();
    assert(hand.totalHan == 1);

    std::cout << "Tiles tests passed\n";
    return 0;
}
