cutechess-cli -engine cmd=$1 -engine cmd=$2 \
    -each tc=10+0.1 proto=uci restart=on \
    -rounds 128000 -games 2 -concurrency 14 \
    -sprt elo0=0 elo1=10 alpha=0.05 beta=0.05 \
    -openings file=8moves_v3.pgn plies=8 order=random