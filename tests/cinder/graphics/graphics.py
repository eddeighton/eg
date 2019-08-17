

import pyeg
import random

r = pyeg.root();

r.screen( [ 800, 800 ] );

def randNum():
    return random.uniform( 0, 1 )

def randPos():
    return [ random.uniform( 200, 800 ), random.uniform( 200, 800 ) ]

def randDir():
    return [ random.uniform( -100, 100 ), random.uniform( -100, 100 ) ]

def randomSequence():
    s = [];
    for i in range( 4 ):
        s.append( 0 )
        s.append( 1 )
        s.append( 2 )
        s.append( 3 )
        s.append( 4 )
    random.shuffle( s )
    random.shuffle( s )
    random.shuffle( s )
    return s
    
for l in r.Line.Raw():
    l.width( 4 )
    l.colour( [ randNum(), randNum(), randNum() ] )
    l.start( randPos() )
    l.end( l.start() + randDir() )
    l.Distance( 20 )
    l.Speed( 100 )
    l.Sequence( randomSequence() )


for i in range( 128 ):
    l = r.Line.Start()
    l.Sequencer.Start()
