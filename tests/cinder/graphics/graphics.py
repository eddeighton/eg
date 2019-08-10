

import pyeg
import random

r = pyeg.root();

r.screen( [ 800, 800 ] );

def randNum():
    return random.uniform( 0, 1 )

def randPos():
    return [ random.uniform( 300, 1800 ), random.uniform( 300, 1000 ) ]

def randDir():
    return [ random.uniform( -100, 100 ), random.uniform( -100, 100 ) ]

def randomSequence():
    s = [];
    for i in range( 100 ):
        s.append( 0 )
        s.append( 1 )
        s.append( 2 )
        s.append( 3 )
        s.append( 4 )
        s.append( 5 )
        s.append( 6 )
        s.append( 7 )
    random.shuffle( s )
    return s
    
for l in r.Line.Raw():
    l.width( 4 )
    l.colour( [ randNum(), randNum(), randNum() ] )
    l.start( randPos() )
    l.end( randPos() )
    l.Distance( 100 )
    l.Speed( 400 )
    l.Sequence( randomSequence() )


for i in range( 16 ):
    l = r.Line.Start()
    l.Sequencer.Start()
