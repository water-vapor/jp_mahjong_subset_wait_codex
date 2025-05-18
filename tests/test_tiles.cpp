#include "tiles.h"
#include "winning_hand.h"
#include <cassert>
#include <iostream>

int main(){
    // tile index helpers
    assert(Tiles::tileIndex("1m") == 0);
    assert(Tiles::tileIndex("9s") == 26);
    assert(Tiles::tileIndex("7z") == 33);
    assert(Tiles::isYaochu(m1));
    assert(!Tiles::isYaochu(p2));

    // test initGameStatus with multiple red fives
    Tiles aka{"5m5p"};
    initGameStatus("1m", aka, "1z", "2z");
    assert(gameInfo.red5m && gameInfo.red5p && !gameInfo.red5s);

    // basic add/remove/count behaviour
    Tiles t;
    assert(t.totalTiles() == 0);
    t.add(m2, 2);
    t.add("3p");
    assert(t.getCount(m2) == 2);
    t.remove("2m");
    assert(t.getCount(m2) == 1);
    assert(t.totalTiles() == 2);
    assert(t.toString() == std::string{"2m3p"});

    std::cout << "Tiles tests passed\n";
    return 0;
}
