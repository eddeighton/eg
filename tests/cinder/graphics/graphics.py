

import pyeg
import random

r = pyeg.root();

r.screen( [ 800, 800 ] );

def randNum():
    return random.uniform( 0, 1 )
    
def randPos():
    return [ random.uniform( 0, 800 ), random.uniform( 0, 800 ) ]
    
def randDir():
    return [ random.uniform( -100, 100 ), random.uniform( -100, 100 ) ]

for l in r.Line.Raw():
    l.width( 10 )
    l.colour( [ randNum(), randNum(), randNum() ] )
    l.start( randPos() )
    l.end( randPos() )
    for m in l.Move.Raw():
        m.dir( randDir() )
        
for i in range( 256 ):
    l = r.Line.Start()
    l.Move.Start();