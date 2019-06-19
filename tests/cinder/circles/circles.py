
import pyeg

r = pyeg.get_root()

pyeg.sleep()

print( "creating circles" )

for i in range( 256 ):
    if i % 2 == 0:
        c = r.Circle.Start()
        c.Morph.Start()
        c.size( 50 )
        c.ColorChanger.Start()
    else:
        c = r.StrokedCircle.Start()
        c.Morph.Start()
        c.size( 75 )
        c.ColorChanger.Start()
   

#sp = r.Spiral.Start()
#sp.curve( 4.0 )
#sp.radius( 100.0 )
#sp.relative( 10.0 )
#sp.speed( 0.2 )