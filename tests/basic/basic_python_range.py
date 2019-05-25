
import pyeg

print( "python started" )

pyeg.sleep() 

print( "python resumed" )

r = pyeg.get_root()

r.A();
r.A();
r.A();

pyeg.sleep() 

for a in r.A.Range():
    print( "got", a )

print( "got to the end" )