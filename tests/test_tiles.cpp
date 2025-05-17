#include "tiles.h"
#include <cassert>
#include <iostream>

int main(){
    assert(Tiles::tileIndex("1m") == 0);
    assert(Tiles::tileIndex("9s") == 26);
    assert(Tiles::isYaochu(m1));
    assert(!Tiles::isYaochu(p2));
    std::cout << "Tiles tests passed\n";
    return 0;
}
