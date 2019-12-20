

import random

a = this.Automata()

table = a.table

for y in range( 0, 4 ):
    for x in range( 0, 4 ):
        for a in range( 0, 4 ):
            actions = table()[ x + ( 4 * y ) ]
            #print( x + ( 4 * y ), x, y, a, actions[ a ] ) 


print( "Test script running" )

numPieces = 12
pieces = []
states = []

for i in range( numPieces ):
    p = [ 0, 0 ]
    piece = this.Piece.Start()
    piece.pos( p )
    piece.movetime( .1 )
    
    pieces.append( piece )
    states.append( 0 )


for i in range( 50 ):

    waits = []
    for i,piece in enumerate( pieces ):
    
        state = states[i]
        actions = table()[ state ]
        
        action = random.randint( 0, 3 )
        while actions[ action ] == -1:
            action = random.randint( 0, 3 )
        newState = actions[ action ]
        
        #print( "state", state, "action", action, "newState", newState, "pos", piece.pos() )
        
        states[ i ] = newState
        
        if action is 0:
            w = piece.Up.Start()
            waits.append( w )
        elif action is 1:
            w = piece.Down.Start()
            waits.append( w )
        elif action is 2:
            w = piece.Left.Start()
            waits.append( w )
        elif action is 3:
            w = piece.Right.Start()
            waits.append( w )
        
    pyeg.sleep( waits )
        
for piece in pieces:
    piece.Stop()

print( "Test script complete" )