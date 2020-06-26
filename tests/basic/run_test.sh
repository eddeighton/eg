#!/bin/bash

filter=$1

for f in test_*${filter}*
do
    echo Test: $f
    cd $f
    eg --build
    eg --run 
    eg --log fail
    cd ..
done
