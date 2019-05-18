
import sys
import pyeg

r = pyeg.get_root()

for i in range( 9 ):
    r.CreateCube();
    pyeg.sleep_seconds( 0.5 )