


print( "Test script running" )

for p in this.root.Get().Piece.Range():
    p.Stop()

s = 4
speed = 0.2

pieces = []

for x in range( -s, s, 2 ):
    for y in range( -s, s, 2 ):
        p = this.Piece.Start()
        p.pos( [ x, y ] )
        p.movetime( speed )
        pieces.append( p )

start = pyeg.ct()

while pyeg.ct() < start + 5:
        
    moves = []
    for p in pieces:
        moves.append( p.Up.Start() )
    pyeg.sleep( moves[ 0 ] )
        
    moves = []
    for p in pieces:
        moves.append( p.Right.Start() )
    pyeg.sleep( moves[ 0 ] )
        
    moves = []
    for p in pieces:
        moves.append( p.Down.Start() )
    pyeg.sleep( moves[ 0 ] )
        
    moves = []
    for p in pieces:
        moves.append( p.Left.Start() )
    pyeg.sleep( moves[ 0 ] )

for p in pieces:
    p.Stop()
    
    
print( "Test script complete" )