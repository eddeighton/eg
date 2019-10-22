
import random
import numpy

print( "Test script running" )

for p in this.root.Get().Piece.Range():
    p.Stop()

s = 8
speed = 0.1
speedExtra = 0.0
pieces = []
index = 0

for x in range( -s, s, 2 ):
    for y in range( -s, s, 2 ):
        index = index + 1
        p = this.Piece.Start()
        p.pos( [ x, y ] )
        p.movetime( speed + speedExtra * index )
        pieces.append( p )

start = pyeg.ct()

pother = this.Piece.Start()
pother.pos( [ -s -4, -s ] )
pother.movetime( 20 )
pother.Move.Start( [ -s -4, -s ], [ -s -4, s - 1 ] )

while pyeg.ct() < start + 4:
        
    seq = [ 0, 0, 0, 2, 2, 2, 1, 1, 1, 3, 3, 3 ]
        
    plans = []
    for p in pieces:
        plan = p.Plan.Start()
        #random.shuffle( seq )
        plan.plan( seq )
        #seq = numpy.roll( seq, 1 )
        plans.append( plan )
    pyeg.sleep( plans )

for p in pieces:
    p.Stop()
    
pother.Stop()
    
print( "Test script complete" )