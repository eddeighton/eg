#!/bin/bash


for f in test_*
do
    echo Test: $f
    cd $f
    eg --build quick
    eg --run
    eg --log fail
    cd ..
done

