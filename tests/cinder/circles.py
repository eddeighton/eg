
import sys
import pyeg
import time

r = pyeg.get_root()

print( "creating circles" )

for i in range( 2048 ):
    c = r.Circle()
    cc = c.ColorChanger()

sp = r.Spiral()
sp.curve( 0.05 )
sp.speed( 10.0 )
