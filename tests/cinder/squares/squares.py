
import pyeg
import math

r = pyeg.root()

pyeg.sleep()

for x in range( 20 ):
    for y in range( 20 ):
        s = r.Square.Start()
        s.size( 20 )
        s.color( ( math.sqrt( x ) * math.sqrt( y ) ) / 20 )
        s.position( [ 300 + x * 20, 300 + y * 20 ] )

print( "Hello squares" )