#!/bin/bash

f=$1

NAME=`echo "$f" | cut -d'.' -f1`

EXE=${NAME}.exe

echo Test: $NAME
eglog --data ${NAME}_build/database.db --log ${NAME}_log fail pass

