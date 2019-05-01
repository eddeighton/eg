#!/bin/bash


for f in *.eg
do
    NAME=`echo "$f" | cut -d'.' -f1`
    
    echo Test: $NAME
    eg --host basic_host $f
    
    #EXE=${NAME}.exe
    ./${NAME}.exe --database ./${NAME}_build/database.db --python ./${NAME}.py
    
    eglog --data ${NAME}_build/database.db --log ${NAME}_log --filter fail
    
done

