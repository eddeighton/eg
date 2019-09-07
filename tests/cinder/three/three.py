


import pyeg

r = pyeg.root()

camOrbit = r.Camera.Orbit.Start();
camOrbit.rate( 0.1 )
camOrbit.radius( 6 )
camOrbit.offset( [0, 16, 0 ] )

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


    
