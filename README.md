# Mahjong Subset Wait Search

This project enumerates all 13-tile subsets from a provided superset and checks whether adding any single tile forms a winning hand. Results are sorted by Han.

## Build

```bash
make
```

## Run Tests

```bash
make test
```

## Usage

```
./main <tile_collection> <dora_indicator> <akadora_indicators> <wind_indicator> <seat_wind_indicator> [limit]
```

Example:

```bash
./main 112233m112233p11s 4z 5m5p 1z 2z
```

If the optional `limit` argument is provided, only that many top scoring hands
and waits will be printed.

This example searches all 13-tile subsets from `112233m112233p11s` using `4z` as the dora indicator, red fives in man and pin suits, prevailing wind `1z` and seat wind `2z`.
