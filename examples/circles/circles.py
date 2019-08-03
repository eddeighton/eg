
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

totalThings = 1024

for i in range( totalThings ):
    c = r.StrokedCircle.Start()
    c.Perturbe.Start();
    c.Morph.Start()
    c.size( 30 )
    c.ColorChanger.Start()
    c.SizeChanger.Start()
    c.SizeChanger.amt( 20 )
    c.SizeChanger.Triangle.rate( 1.0 )

r.Spiral.Start()
r.Spiral.Stop()

r.setPerturbe.iRate( 0.2 )
r.setPerturbe.iRange( 50 )
r.setPerturbe();