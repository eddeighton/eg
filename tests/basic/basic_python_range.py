
import pyeg
import time

r = pyeg.get_root()

r.A();
r.A();
r.A();

time.sleep( 0.5 ) 

for a in r.A.Range():
    print( "got", a )

print( "got to the end" )