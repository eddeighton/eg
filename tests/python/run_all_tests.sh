#!/bin/bash

for f in *
do
    if [ -d "$f" ]; then
        echo Test: $f
        cd $f
        eg --build quick
        eg --run
        eg --log fail
        cd ..
    fi
done