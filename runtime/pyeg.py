
#import pyeg
#import IPython
#IPython.start_kernel()
#pyeg.testMethod()

import eg
import eg_host


root = eg.root()
print( "root is: ", root )

r2 = root.a
print( "r2 is: ", r2 )

r3 = r2.b.c
print( "r3 is: ", r3 )



root()
print( "root is: ", root )

r2()
print( "r2 is: ", r2 )

r3()
print( "r3 is: ", r3 )



root()
print( "root is: ", root )

r2()
print( "r2 is: ", r2 )

r3()
print( "r3 is: ", r3 )



print( "Got the root: " )
print( "Test Host Function: ", eg_host.testHostFunction( root ) )
print( "Script complete" )

#print( "Script started" )

#print("numargs returned: ", emb.numargs( 9023, 0.01 ))

#print( "testHostFunction returned: ", eg_host.testHostFunction( 2, 3.4 ) )

#