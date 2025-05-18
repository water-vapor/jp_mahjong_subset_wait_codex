#include "search.h"
#include <cassert>
#include <sstream>
#include <iostream>

int main(){
    // prepare result map with one zero-han and one positive-han entry
    ResultMap res;
    Tiles h1{"123m123p123s11z"};
    int wait1 = Tiles::tileIndex("4m");
    res[h1][wait1] = WaitEntry{wait1, 0, "none"};

    Tiles h2{"123m123p123s22z"};
    int wait2 = Tiles::tileIndex("5m");
    res[h2][wait2] = WaitEntry{wait2, 2, "tanyao"};

    std::ostringstream oss;
    auto *orig = std::cout.rdbuf(oss.rdbuf());
    printResults(res);
    std::cout.rdbuf(orig);

    std::string out = oss.str();
    // Output should contain both hands due to the wait count section
    assert(out.find(h2.toString()) != std::string::npos);
    assert(out.find(h1.toString()) != std::string::npos);
    // Verify localization and separator
    assert(out.find("番") != std::string::npos);
    assert(out.find("-----") != std::string::npos);
    // Ensure the English word "han" is not printed
    assert(out.find("han") == std::string::npos);

    std::cout << "Print tests passed\n";
    return 0;
}
