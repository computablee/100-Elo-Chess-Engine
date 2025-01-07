cutechess-cli -engine cmd=$1 -engine cmd=$2 \
    -each tc=120+1 proto=uci restart=on \
    -rounds 500 -games 2 -concurrency 14 \
    -openings file=8moves_v3.pgn plies=8 order=random