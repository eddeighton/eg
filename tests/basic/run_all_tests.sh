#!/bin/bash


for f in *.eg
do
    NAME=`echo "$f" | cut -d'.' -f1`
    
    #echo Found $NAME
    eg --host basic_host $f
    
    EXE=${NAME}.exe
    eval "./$EXE"
    
    echo Test: $NAME
    eglog --data ${NAME}_build/database.db --log ${NAME}_log --filter fail
    
done

