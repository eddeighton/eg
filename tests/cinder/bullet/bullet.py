
import pyeg

r = pyeg.root()

#camOrbit = r.Camera.Orbit.Start()
#camOrbit.rate( 0.1 )
#camOrbit.radius( 6 )
#camOrbit.offset( [0, 16, 0 ] )

b = r.Ball.Start()
b.radius( 0.5 )
b.colour( [1,0,0] )

gridsize = 4
linewidth = 2
linecolour = [ 0.1, 0.4, 0.5 ]


r.Mouse.Start()
r.Camera.CameraMouse.radius( 10 )
r.Camera.CameraMouse.rate( [ 0.01, 0.4 ] )

r.Camera.Orbit.orbit_rate( 0.2 )
r.Camera.Orbit.orbit_radius( 6 )
r.Camera.Orbit.orbit_magic( 3 )


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


    
