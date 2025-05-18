#include "tiles.h"
#include "winning_hand.h"
#include "search.h"
#include <iostream>
#include <string>

int main(int argc, char *argv[]){
    if (argc != 6 && argc != 7)
    {
        std::cerr << "Usage: " << argv[0]
                  << " <tile_collection> <dora_indicator> <akadora_indicators> <wind_indicator> <seat_wind_indicator> [limit]\n";
        return 1;
    }

    Tiles allHand;
    std::string doraIndicator;
    Tiles akadoraIndicators;
    std::string windIndicator;
    std::string seatWindIndicator;
    size_t limit = 0;

    try
    {
        allHand = Tiles(argv[1]);
        doraIndicator = argv[2];
        akadoraIndicators = Tiles(argv[3]);
        windIndicator = argv[4];
        seatWindIndicator = argv[5];
        if (argc == 7) {
            limit = std::stoul(argv[6]);
        }
        initGameStatus(doraIndicator, akadoraIndicators, windIndicator, seatWindIndicator);
    }
    catch (std::exception const &e)
    {
        std::cerr << "Parsing error: " << e.what() << "\n";
        return 1;
    }

    ResultMap results;
    searchHands(allHand, results);
    printResults(results, limit);
    return 0;
}
