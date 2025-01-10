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

Currently, the 100 Elo Chess Engine is using the [Stash engine](https://gitlab.com/mhouppin/stash-bot) and [cutechess](https://cutechess.com/) to get a rough idea of Elo.
We have compared to:

- v9 (1287 Elo)
- v11 (1698 Elo)
- v12 (1891 Elo)
- v13 (1977 Elo)
- v14 (2068 Elo)
- v15 (**estimated** 2150 Elo)

Latest run was done against `v15` with a time control of `120+1` (in seconds) with `1000` total runs.

Results may be outdated.
I run these occasionally; the engine may be stronger than the results shown.

```
Score of 100 Elo Chess Engine vs Stash v15.0: 287 - 335 - 378  [0.476] 1000
...      100 Elo Chess Engine playing White: 147 - 147 - 206  [0.500] 500
...      100 Elo Chess Engine playing Black: 140 - 188 - 172  [0.452] 500
...      White vs Black: 335 - 287 - 378  [0.524] 1000
Elo difference: -16.7 +/- 17.0, LOS: 2.7 %, DrawRatio: 37.8 %
```

This results in an estimated Elo of: `2133`.

# Current Features

The 100 Elo Chess Engine uses the following architecture:

- Basic/buggy UCI support
- PeSTO with incremental evaluation
- Iterative deepening
- Transposition table
- Null move pruning
- Move ordering (best move -> MVV-LVA -> killer moves -> quiet moves)
- Quiescent search
- Principal variation search
- Late move reduction
- Check extensions
- Internal iterative reductions