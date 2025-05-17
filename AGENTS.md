# Setup
 - This is a C++ project with no external dependencies.
 - Build the main binary with `make`.
 - Run unit tests with `make test`.

# Problem Statement
 - You are playing japanese mahjong in a special setup.
 - You are given a set of 34 (or similar number) mahjong tiles, and you wish to choose a hand of 13 tiles that forms a Wait.
 - Preferrably, you want the Han you can obtain to as high as possible, so you need to search for all possibilities, rank them, and then sort by the Han.

# What has been done
 - The basic design of mahjong representation is from Tenhou. The `Tile` class is a general map that counts each tile, while the winning hand stores either 7-pair, or 4 groups and 1 pair in an efficient way.
 - The testing or yaku, yakuman, local yakus are implemented as methods of winning hands.
 - The cli reading is implemented, it takes the superset (of tiles), dora indicator, aka dora presence in the tiles, and the previling and seat winds.

# What has not been done
 - The result storing map. It should contain the 13-hand as key, and for each value, you need to have a nested map (since there can be multiple waits) containing of the tile to wait (perhaps as the key), score, yaku string (these two as the value). Note that if there are multiple interpretations of the set 14-set tile, they will result in different WinningHands classes, but will not duplicate in this map since their keys should be the same (same 13-tile set and same wait),so only need to store the interpretation with highest score.
 - The DFS searching logic.
     - In my design, first we should check for Kokushi (13 yao) or Kokushi Jusanmen (13 different + any tile), since it does not form a valid WinningHand in the design, and are easy to check
     - Then check for 7-pairs, we can filter all tiles and only consider these with pairs. Do a DFS on the 6-pair combination, then combine the selected 6-pair with any (different) tile available.
     - Then check for regular 4 group+1pair combination. In this case, you should consider (a). waiting one tile in the pair, so you just do dfs selection for 4 group; (b) waiting for one tile in a group, more considerates here. But generally, you can select unique (if they are present) groups by increasing the beginning tile and group type.

 - The final sorting and pretty printing system

# Contributor Guide
1. You can reorganize the project into multiple files following C++ project conventions. If there are variable, files naming or English issues, you should fix them (including this file)
2. You should write test cases for small components, especially for yaku related testing, as they are prune to errors.
3. For all yaku testing, currently they modify in-class variables directly, make them return han and yaku string, only modify variables in the calculateHan and related functions.
4. You should check for duplicated, or inelegant code, if there are betters and cleaner ways to write it, you can change it.
