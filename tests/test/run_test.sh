#!/bin/bash

f=$1

NAME=`echo "$f" | cut -d'.' -f1`

echo Test: $NAME
eg --host test_host $f

EXE=${NAME}.exe
eval "./$EXE"

eglog --data ${NAME}_build/database.db --log ${NAME}_log --filter fail


