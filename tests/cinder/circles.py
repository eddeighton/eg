
import pyeg

r = pyeg.get_root()

pyeg.sleep()

print( "creating circles" )

for i in range( 256 ):
    c = r.Circle.Start()
    c.ColorChanger.Start()
   

sp = r.Spiral.Start()
sp.curve( 0.25 )
sp.speed( 10.0 )
