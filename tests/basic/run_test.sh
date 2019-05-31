#!/bin/bash

f=$1

NAME=`echo "$f" | cut -d'.' -f1`

echo Test: $NAME
eg --host basic_host $f

./${NAME}.exe --database ./${NAME}_build/database.db --python ./${NAME}.py

eglog --data ${NAME}_build/database.db --log ${NAME}_log --filter fail


