#!/bin/bash

for fl in test_*
do
    cd $fl
    for f in *.db *.pch *.gch *.o *.obj *.exe *.tmp *.egcpp *.log *.pdb *.ilk *.xml *interface* *operations* *impl* *includes* *build *log
    do
        if [ -d "$f" ]; then
            rm -rf $f
        else
            [ -e "$f" ] || continue
            echo Removing $f
            rm $f
        fi
    done
    cd ..
done