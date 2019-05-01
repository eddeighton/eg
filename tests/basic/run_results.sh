#!/bin/bash


for f in *.eg
do
    NAME=`echo "$f" | cut -d'.' -f1`
    
    EXE=${NAME}.exe
    
    echo Test: $NAME
    eglog --data ${NAME}_build/database.db --log ${NAME}_log fail pass
    
done

