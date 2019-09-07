


print( "Test script running" )

for p in this.root.Get().Piece.Range():
    p.Stop()

s = 4
speed = 0.1

pieces = []

for x in range( -s, s, 2 ):
    for y in range( -s, s, 2 ):
        p = this.Piece.Start()
        p.pos( [ x, y ] )
        p.movetime( speed )
        pieces.append( p )

start = pyeg.ct()

pother = this.Piece.Start()
pother.pos( [ -s -1, -s ] )
pother.movetime( 8 )
pother.Move.Start( [ -s -1, -s ], [ -s -1, s - 1 ] )

while pyeg.ct() < start + 8:
        
    moves = []
    for p in pieces:
        moves.append( p.Up.Start() )
    pyeg.sleep( moves )
        
    moves = []
    for p in pieces:
        moves.append( p.Right.Start() )
    pyeg.sleep( moves )
        
    moves = []
    for p in pieces:
        moves.append( p.Down.Start() )
    pyeg.sleep( moves )
        
    moves = []
    for p in pieces:
        moves.append( p.Left.Start() )
    pyeg.sleep( moves )

for p in pieces:
    p.Stop()
    
pother.Stop()
    
print( "Test script complete" )