# Build

```sh
# Clone repo and submodules
git clone https://github.com/computablee/100-Elo-Chess-Engine.git
cd 100-Elo-Chess-Engine
git submodule init
git submodule update

# Build
mkdir build
cd build
cmake ..
make -j8
```

# Current Elo Estimates

Currently, the 100 Elo Chess Engine is using the Stash project and cutechess to get a rough idea of Elo.
We have compared to:

- v9 (1275 Elo)
- v11 (1690 Elo)
- v12 (1886 Elo)
- v13 (1972 Elo)

Latest run was done against `v12` with a time control of `10+0.1` (in seconds) with `250` total runs.

Results may be outdated.
I run these occasionally; the engine may be stronger than the results shown.

```
Score of 100 Elo Chess Engine vs Stash v12.0: 73 - 90 - 87  [0.466] 250
Elo difference: -23.5 +/- 34.8, LOS: 9.3 %, DrawRatio: 34.5 %
```

This results in an estimated Elo of: `1863`.

# Current Features

The 100 Elo Chess Engine uses the following architecture:

- Basic/buggy UCI support
- PeSTO evaluation
- Iterative deepening
- Transposition table
- Null move pruning
- Move ordering (best move, then MVV-LVA for captures)
- Quiescent search