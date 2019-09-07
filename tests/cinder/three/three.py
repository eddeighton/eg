


import pyeg

r = pyeg.root()

camOrbit = r.Camera.Orbit.Start();
camOrbit.rate( 0.2 )
camOrbit.radius( 4 )
camOrbit.offset( [0, 5, 0 ] )

#grid
for x in range( -4, 5 ):
    l = r.BoardLine.Start()
    l.start( [ x, 0, -4 ] )
    l.end(   [ x, 0, 4 ] )
    l.colour( [ 0.5, 0.5, 0.5 ] )
    l.width( 1 )
        
for z in range( -4, 5 ):
    l = r.BoardLine.Start()
    l.start( [ -4, 0, z ] )
    l.end(   [ 4, 0, z ] )
    l.colour( [ 0.5, 0.5, 0.5 ] )
    l.width( 1 )


p1 = r.Piece.Start()
p1.pos( [ 0, 0 ] )

p2 = r.Piece.Start()
p2.pos( [ 1, 0 ] )

p3 = r.Piece.Start()
p3.pos( [ 0, 1 ] )
    
