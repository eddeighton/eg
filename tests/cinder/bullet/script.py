
import random
import numpy

print( "Test script running" )

numPieces = 32
pieces = []

for i in range( numPieces ):
    p = [ 0, 0 ]
    piece = this.Piece.Start()
    piece.pos( p )
    piece.movetime( .5 )
    
    pieces.append( piece )

history = [ 1, 0, 0, 0, 0, 0, 0, 0, 0 ]

state = 0

for i in range( 20 ):

    waits = []
    for i,piece in enumerate( pieces ):
        w = piece.Move.Start( 
            [ random.randint( -8, 8 ),
              random.randint( -8, 8 ) ] )
        waits.append( w )
        
    pyeg.sleep( waits )
        
for piece in pieces:
    piece.Stop()

print( "Test script complete" )