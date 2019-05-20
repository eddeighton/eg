
import sys
import pyeg

r = pyeg.get_root()

pyeg.sleep_seconds( 0.01 )
        
for y in range( 10 ):
    for i in range( 9 ):
        r.CreateCube();
        pyeg.sleep_seconds( 0.05 )
        
    for c in r.Cube.Range():
        c.Stop()
        pyeg.sleep_seconds( 0.05 )
        