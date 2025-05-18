#include "search.h"
#include <cassert>
#include <iostream>

void checkSuperset(const std::string &str){
    Tiles superset{str};
    ResultMap res;
    searchHands(superset, res);
    assert(!res.empty());
}

int main(){
    // original basic test
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

    // larger supersets to test performance
    checkSuperset("123456789m123456789p12s");          // 20 tiles
    checkSuperset("123456789m123456789p12345s12z");    // 25 tiles
    checkSuperset("123456789m123456789p123456789s123z"); // 30 tiles
    checkSuperset("123456789m123456789p123456789s1234567z"); // 34 tiles

    std::cout << "Search tests passed\n";
    return 0;
}
