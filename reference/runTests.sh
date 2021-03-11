

echo "Building and Running all tests"

cd ../tests/reference

CDIR=$(pwd)
for i in $(ls -R | grep :); do
    DIR=${i%:}                    # Strip ':'
    cd $DIR
    
    if [ -f ".eg" ]; then
        echo "Found Test: " $DIR
        
        eg --build && eg --run && eg --log
        
    fi
    
    cd $CDIR
done
