


import pyeg

r = pyeg.root()

#camOrbit = r.Camera.Orbit.Start()
#camOrbit.rate( 0.1 )
#camOrbit.radius( 6 )
#camOrbit.offset( [0, 16, 0 ] )

mouse = r.Camera.Mouse.Start()
mouse.radius( 10 )
mouse.rate( [ 0.01, 0.4 ] )

gridsize = 10
linewidth = 5
linecolour = [ 0.1, 0.4, 0.5 ]

#grid
for x in range( -gridsize, gridsize + 1 ):
    l = r.BoardLine.Start()
    l.start( [ x, 0, -gridsize ] )
    l.end(   [ x, 0, gridsize ] )
    l.colour( linecolour )
    l.width( linewidth )
        
for z in range( -gridsize, gridsize + 1 ):
    l = r.BoardLine.Start()
    l.start( [ -gridsize, 0, z ] )
    l.end(   [ gridsize, 0, z ] )
    l.colour( linecolour )
    l.width( linewidth )


    
