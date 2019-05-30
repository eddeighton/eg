
import pyeg

r = pyeg.get_root()

pyeg.sleep()
pyeg.sleep()
pyeg.sleep()
pyeg.sleep()

print( "creating circles" )

for i in range( 2047 ):
    c = r.Circle.Start()
    pyeg.sleep()
    c.ColorChanger.Start()
    pyeg.sleep()
    
sp = r.Spiral.Start()
sp.curve( 0.05 )
sp.speed( 10.0 )
