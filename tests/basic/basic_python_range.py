
import pyeg

print( "python started" )

r = pyeg.get_root()

r.A();

pyeg.sleep( 0.5 )

r.A();

for a in r.A.Range():
    print( "got", a )
    
while r.Done() is False:
    pyeg.sleep()

print( "python got to the end" )