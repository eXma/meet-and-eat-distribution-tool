#!/bin/bash

set -e

cd $HOME/run

for f in distances teams; do
    cp "/plandata/${f}_${1}.json" .
done

LD_PRELOAD=/usr/lib/libtcmalloc_and_profiler.so.4  /usr/bin/python3.4 -OO  integration_test.py $1 | tee /plandata/output_$1

