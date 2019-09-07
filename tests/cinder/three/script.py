
import random
import numpy

print( "Test script running" )

for p in this.root.Get().Piece.Range():
    p.Stop()

s = 8
speed = 1
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

while pyeg.ct() < start + 10:
        
    seq = [ 0, 2, 1, 3, 0, 2, 1, 3 ]
    #seq = [ 0, 1, 0, 1 ]
    #random.shuffle( seq )
        
    plans = []
    for p in pieces:
        plan = p.Plan.Start()
        plan.plan( seq )
        #seq = numpy.roll( seq, 1 )
        plans.append( plan )
    pyeg.sleep( plans )

for p in pieces:
    p.Stop()
    
pother.Stop()
    
print( "Test script complete" )