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
    auto wit = it->second.find(wait);
    assert(wit != it->second.end());
    assert(wit->second.han > 0);
    std::cout << "Search tests passed\n";
    return 0;
}
