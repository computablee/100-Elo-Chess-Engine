# Build

The CMake build scripts are only tested on Linux using the GNU C++ compiler (for native compilation) and the MinGW C++ compiler (for cross-compilation to Windows).
MacOS (with Apple Silicon) seems to work in my limited testing, since `g++` on MacOS is just a link to the LLVM C++ compiler.

To be honest, I have never used CMake on Windows, so I don't even know how to start with that.

```sh
# Clone repo and submodules
git clone https://github.com/computablee/100-Elo-Chess-Engine.git
cd 100-Elo-Chess-Engine
git submodule init
git submodule update

# Build
mkdir build
cd build
cmake .. # -DWINDOWS=true to cross-compile to Windows
make     # -jN for multi-core compilation
```

# Current Elo Estimates

**Update**: The 100 Elo Chess Engine has been released to CCRL for evaluation.
This README will be updated with a rating as soon as I'm aware of it.

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
Score of 100 Elo Chess Engine vs Stash v15.0: 331 - 275 - 394  [0.528] 1000
...      100 Elo Chess Engine playing White: 181 - 114 - 205  [0.567] 500
...      100 Elo Chess Engine playing Black: 150 - 161 - 189  [0.489] 500
...      White vs Black: 342 - 264 - 394  [0.539] 1000
Elo difference: 19.5 +/- 16.8, LOS: 98.9 %, DrawRatio: 39.4 %
```

This results in an estimated Elo of: `2170`.

# Current Features

The 100 Elo Chess Engine uses the following architecture:

- Basic/buggy UCI support
- PeSTO with incremental evaluation
- Iterative deepening
- Transposition table
- Null move pruning
- Move ordering (best move -> MVV-LVA -> killer moves -> history heuristic)
- Quiescent search
- Principal variation search
- Late move reduction
- Check extensions
- Internal iterative reductions