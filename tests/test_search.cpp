#include "search.h"
#include <cassert>
#include <iostream>

int main(){
    Tiles superset{"112233m112233p11s"};
    ResultMap res;
    searchHands(superset, res);
    assert(res.size() > 1); // multiple subsets should be evaluated
    Tiles target{"112233m112233p1s"};
    auto it = res.find(target);
    assert(it != res.end());
    int wait = Tiles::tileIndex("1s");
    bool found = false;
    int foundHan = 0;
    for(const auto &we : it->second){
        if(we.waitTile == wait){
            found = true;
            foundHan = we.han;
            break;
        }
    }
    assert(found);
    assert(foundHan > 0);
    std::cout << "Search tests passed\n";
    return 0;
}
