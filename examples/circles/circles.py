
import pyeg

r = pyeg.root()

pyeg.sleep()

print( "creating circles" )

#configure some dimensions - dont need to have started anything yet
#keep all the magic numbers in script...
r.Randomise.range( 1000 )
r.Spiral.speed( 0.05 )
r.Spiral.curve( 0.5 )
r.Spiral.relative( 3 )
r.morphSpeed( 0.75 )

for i in range( 4095 ):
    c = r.StrokedCircle.Start()
    c.Morph.Start()
    c.size( 20 )
    c.ColorChanger.Start()

r.Spiral.Start()