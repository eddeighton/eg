
import random
import numpy

print( "Test script running" )

numPieces = 9
pieces = []

for i in range( numPieces ):
    p = [ i * 3, 0 ]
    piece = this.Piece.Start()
    piece.pos( p )
    piece.movetime( .5 )
    
    pieces.append( piece )


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
        #"c" : 4
    },
    1 : 
    {
        "a" : 0,
        "b" : 2,
        "c" : 4,
        #"d" : 3,
        #"e" : 5
    },
    2 : 
    {
        "a" : 1,
        "b" : 5,
        #"c" : 4,
    },
    3 : 
    {
        "a" : 0,
        "b" : 4,
        "c" : 6,
        #"d" : 1,
        #"e" : 7
    },
    4 : 
    {
        "a" : 1,
        "b" : 3,
        "c" : 5,
        "d" : 7,
        #"e" : 0,
        #"f" : 2,
        #"g" : 6,
        #"h" : 8
    },
    5 : 
    {
        "a" : 2,
        "b" : 4,
        "c" : 8,
        #"d" : 1,
        #"e" : 7
    },
    6 : 
    {
        "a" : 3,
        "b" : 7,
        #"c" : 4
    },
    7 : 
    {
        "a" : 4,
        "b" : 6,
        "c" : 8,
        #"d" : 3,
        #"e" : 5,
    },
    8 : 
    {
        "a" : 5,
        "b" : 7,
        #"c" : 4
    },
}

history = [ 1, 0, 0, 0, 0, 0, 0, 0, 0 ]

state = 0

for i in range( 40 ):

    numOptions = len( delta[ state ] )
    
    choices = []
    lowest = 10000000
    for i in range( numOptions ):
        a = actions[ i ]
        s = delta[ state ][ a ]
        c = history[ s ]
        if c == lowest:
            choices.append( a )
        if c < lowest:
            lowest = c
            choices = []
            choices.append( a )
    

    choice = choices[ random.randint( 0, len( choices ) - 1 ) ]
    
    #posFrom = positions[ state ]
    oldState = state
    state = delta[ state ][ choice ]
    posTo = positions[ state ]
    history[ state ] = history[ state ] + 1
    
    waits = []
    for i,piece in enumerate( pieces ):
        w = piece.Move.Start( posTo )
        posTo = piece.pos()
        waits.append( w )
        
    pyeg.sleep( waits )
    
    #print( oldState, " to ", state, choice, history, delta[ oldState ] )
    #this.PrintPieceState()
        
for piece in pieces:
    piece.Stop()

print( "Test script complete" )