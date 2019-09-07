


print( "Test script running" )

p1 = this.Piece.Start()
p1.pos( [ 0, 0 ] )
p1.movetime( 1 )

p2 = this.Piece.Start()
p2.pos( [ 2, 0 ] )
p2.movetime( 1 )

p3 = this.Piece.Start()
p3.pos( [ 2, 2 ] )
p3.movetime( 1 )

p4 = this.Piece.Start()
p4.pos( [ 0, 2 ] )
p4.movetime( 1 )

pyeg.sleep( 1.0 )

p1.Up.Start()
p2.Up.Start()
p3.Up.Start()
p4.Up.Start()

pyeg.sleep( 1.0 )

p1.Left.Start()
p2.Left.Start()
p3.Left.Start()
p4.Left.Start()

pyeg.sleep( 1.0 )

p1.Down.Start()
p2.Down.Start()
p3.Down.Start()
p4.Down.Start()

pyeg.sleep( 1.0 )

p1.Right.Start()
p2.Right.Start()
p3.Right.Start()
p4.Right.Start()

pyeg.sleep( 1.0 )

p1.Stop()
p2.Stop()
p3.Stop()
p4.Stop()