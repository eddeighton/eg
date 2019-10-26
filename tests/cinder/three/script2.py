
import random
import numpy

print( "Test script running" )


piece = this.Piece.Start()
piece.pos( [ 0, 0 ] )
piece.movetime( .2 )

actions = [ "a", "b", "c", "d", "e", "f", "g", "h" ]

positions = [ 
        [ -3,-3 ], [ 0,-3 ], [ 3,-3 ], 
        [ -3, 0 ], [ 0, 0 ], [ 3, 0 ], 
        [ -3, 3 ], [ 0, 3 ], [ 3, 3 ] 
        ]
    
delta = {
    0 : 
    {
        "a" : 1,
        "b" : 3,
        "c" : 4
    },
    1 : 
    {
        "a" : 0,
        "b" : 2,
        "c" : 3,
        "d" : 4,
        "e" : 5
    },
    2 : 
    {
        "a" : 1,
        "b" : 4,
        "c" : 5
    },
    3 : 
    {
        "a" : 0,
        "b" : 1,
        "c" : 4,
        "d" : 6,
        "e" : 7
    },
    4 : 
    {
        "a" : 0,
        "b" : 1,
        "c" : 2,
        "d" : 3,
        "e" : 5,
        "f" : 6,
        "g" : 7,
        "h" : 8
    },
    5 : 
    {
        "a" : 1,
        "b" : 2,
        "c" : 4,
        "d" : 7,
        "e" : 8
    },
    6 : 
    {
        "a" : 3,
        "b" : 4,
        "c" : 7
    },
    7 : 
    {
        "a" : 3,
        "b" : 4,
        "c" : 5,
        "d" : 6,
        "e" : 8
    },
    8 : 
    {
        "a" : 4,
        "b" : 5,
        "c" : 7
    },
}

state = 0

for i in range( 10 ):
    choice = actions[ random.randint( 0, len( delta[ state ] ) - 1 ) ]
    posFrom = positions[ state ]
    state = delta[ state ][ choice ]
    posTo = positions[ state ]
    piece.Move( posFrom, posTo )
        
piece.Stop()
#m = piece.Move.Start( [ 0, 0 ], [ 1, 0 ] )

this.SolvePieces()
pyeg.sleep()
this.SolvePieces()
    
print( "Test script complete" )