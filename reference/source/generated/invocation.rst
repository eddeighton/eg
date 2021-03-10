##########
invocation
##########
****
data
****
=====
Basic
=====
-------
Reading
-------

To read or write dimensions in EG one must use the invocation system.
Reading is the simplest operation type where the context is invoked similar to invoking the C++ application operator.


example::

    action Test[ 4 ]
    {
        dim int x; //basic integer dimension
        dim int y; //basic integer dimension
        dim int z; //basic integer dimension
    }
    Test test = Test(); //get a reference to a Test
    //now read x, y, z
    TEST( test.x() == 0 );
    TEST( test.y() == 0 );
    TEST( test.z() == 0 );

---------------
DimensionTraits
---------------

The actual return type of the Read operation is defined via the DimensionTraits for the underlying data type.  This will default to a const reference unless overridden by the user


example::

    const int& iref = test.x(); //no copy made of the underlying data.

-------
Writing
-------

In order to write one uses the same syntax as reading but passes a value.


example::

    test.x( 123 );
    TEST( test.x() == 123 );
    TEST( test.y() == 0 );
    TEST( test.z() == 0 );

^^^^^^^^
Chaining
^^^^^^^^

When writing the return value will actually be a reference to the parent context of the dimension so that further invocations can be convieniently chained as in the following.


example::

    test.x( 234 ).y( 345 ).z( 456 );
    TEST( test.x() == 234 );
    TEST( test.y() == 345 );
    TEST( test.z() == 456 );
    //Can even invoke a new action and initialise using chaining
    Test test2 = Test().x( 1 ).y( 2 ).z( 3 );
    TEST( test2.x() == 1 );
    TEST( test2.y() == 2 );
    TEST( test2.z() == 3 );

Full example program source code at: https://github.com/eddeighton/eg/tree/master/tests/reference/ccc_invocation/aaa_data

Program Output:

+---------+-----+---------------+--------+
|Timestamp|Type |Value          |Instance|
+=========+=====+===============+========+
|00000001 |START|root           |00000000|
+---------+-----+---------------+--------+
|00000001 |START|root_Test      |00000000|
+---------+-----+---------------+--------+
|00000001 |PASS |test.x() == 0  |        |
+---------+-----+---------------+--------+
|00000001 |PASS |test.y() == 0  |        |
+---------+-----+---------------+--------+
|00000001 |PASS |test.z() == 0  |        |
+---------+-----+---------------+--------+
|00000001 |PASS |test.x() == 123|        |
+---------+-----+---------------+--------+
|00000001 |PASS |test.y() == 0  |        |
+---------+-----+---------------+--------+
|00000001 |PASS |test.z() == 0  |        |
+---------+-----+---------------+--------+
|00000001 |PASS |test.x() == 234|        |
+---------+-----+---------------+--------+
|00000001 |PASS |test.y() == 345|        |
+---------+-----+---------------+--------+
|00000001 |PASS |test.z() == 456|        |
+---------+-----+---------------+--------+
|00000001 |START|root_Test      |00000001|
+---------+-----+---------------+--------+
|00000001 |PASS |test2.x() == 1 |        |
+---------+-----+---------------+--------+
|00000001 |PASS |test2.y() == 2 |        |
+---------+-----+---------------+--------+
|00000001 |PASS |test2.z() == 3 |        |
+---------+-----+---------------+--------+
|00000001 |STOP |root           |00000000|
+---------+-----+---------------+--------+
|00000001 |STOP |root_Test      |00000000|
+---------+-----+---------------+--------+
|00000001 |STOP |root_Test      |00000001|
+---------+-----+---------------+--------+

