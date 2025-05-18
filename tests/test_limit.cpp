#include "search.h"
#include <cassert>
#include <sstream>
#include <iostream>

int main(){
    ResultMap res;
    Tiles h1{"123m123p123s11z"};
    int w1 = Tiles::tileIndex("4m");
    res[h1][w1] = WaitEntry{w1, 2, "t1"};

    Tiles h2{"123m123p123s22z"};
    int w2 = Tiles::tileIndex("5m");
    res[h2][w2] = WaitEntry{w2, 2, "t2"};

    Tiles h3{"123m123p123s33z"};
    int w3 = Tiles::tileIndex("6m");
    res[h3][w3] = WaitEntry{w3, 2, "t3"};

    std::ostringstream oss;
    auto *orig = std::cout.rdbuf(oss.rdbuf());
    printResults(res, 2); // limit to 2 lines
    std::cout.rdbuf(orig);

    std::string out = oss.str();
    size_t countHan = 0;
    size_t pos = 0;
    while ((pos = out.find("番", pos)) != std::string::npos) {
        ++countHan;
        ++pos;
    }
    size_t countWait = 0;
    pos = 0;
    while ((pos = out.find("面待ち", pos)) != std::string::npos) {
        ++countWait;
        ++pos;
    }
    assert(countHan == 2); // limited to two lines
    assert(countWait == 2);
    std::cout << "Limit tests passed\n";
    return 0;
}
