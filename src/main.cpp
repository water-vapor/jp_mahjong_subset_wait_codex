#include "tiles.h"
#include "winning_hand.h"
#include <unordered_map>
#include <iostream>
#include <string>

void test_basic_tiles(){
    Tiles allHand;
    std::string doraIndicator;
    Tiles akadoraIndicators;
    allHand = Tiles("123456789m123456789p123456789s123456789z");
    doraIndicator = "4z";
    akadoraIndicators = Tiles("5m5s");
    Tiles customHand = Tiles();
    // customHand.add(33);
    // customHand.add(33, 2);
    // customHand.add("2m");
    // // customHand.add("2m", 3);
    // customHand.add("2m", 4);
    customHand.remove("2m", 1);
    customHand.add(10);
    customHand.add(0);

    std::unordered_map<Tiles, int, Tiles::Hash> M;
    M.reserve(16);
    M[allHand] = 1;
    M[customHand] = 3;
    M[akadoraIndicators] = 4;
    std::cout << "Full hand size:   " << allHand.totalTiles() << std::endl;
    std::cout << "All hand: " << allHand.toString() << std::endl;
    std::cout << "Dora indicator: " << doraIndicator << std::endl;
    std::cout << "Akadora indicators: " << akadoraIndicators.toString() << std::endl;
    for (auto const &[k, v] : M)
    {
        std::cout << "Key: " << k.toString() << ", Value: " << v << "\n";
    }
    
}


int main(int argc, char *argv[]){
    test_basic_tiles();
    if (argc != 6)
    {
        std::cerr << "Usage: " << argv[0]
                  << " <tile_collection> <dora_indicator> <akadora_indicators> <wind_indicator> <seat_wind_indicator>\n";
        return 1;
    }

    Tiles allHand;
    std::string doraIndicator;
    Tiles akadoraIndicators;
    std::string windIndicator;
    std::string seatWindIndicator;

    try
    {
        allHand = Tiles(argv[1]);
        doraIndicator = argv[2];
        akadoraIndicators = Tiles(argv[3]);
        windIndicator = argv[4];
        seatWindIndicator = argv[5];
        initGameStatus(doraIndicator, akadoraIndicators, windIndicator, seatWindIndicator);
    }
    catch (std::exception const &e)
    {
        std::cerr << "Parsing error: " << e.what() << "\n";
        return 1;
    }

    std::unordered_map<Tiles, int, Tiles::Hash> M;

    // core logic:
    // 
    return 0;
}
