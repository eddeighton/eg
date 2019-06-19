#!/bin/bash

for f in test_*
do
    cd $f
    eg --log fail
    cd ..
done
