

import pyeg

r = pyeg.root()

r.smoothing( 0.01 )
r.colourRate( 0.5 )

for c in r.Circle.Raw():
    c.size( 75 )
    c.lifetime( 4 )
