###########
inheritance
###########
********
concrete
********
=====
basic
=====

A context can inherit from any other context.  Inheritance means the deriving context inherits all features of the underlying type and can then override and extend them


example::

    action A
    {
        dim int x;
        function test() : void 
        {
            LOG( "A::test" );
        }
    }
    action B : A
    {
        dim int y;
        function test() : void 
        {
            LOG( "B::test" );
        }
    }

=========
continued
=========

A context will inherit the underlying domain size of the derived type but can override it

------
detail
------

A context will inherit the underlying domain size of the derived type but can override it


example::

    action C[ 4 ]
    {
        dim int x;
        function test() : void 
        {
            LOG( "C::test" );
        }
    }
    action D : C
    {
        //has domain size of four
        function test() : void 
        {
            LOG( "D::test" );
        }
    };
    action E[ 5 ] : C
    {
        //has domain size of five
        function test() : void 
        {
            LOG( "E::test" );
        }
    };
    std::vector< Var< A, B, C, D, E > > actions = { A(), B(), C(), D(), E() };
    for( auto v : actions )
        v.test();

Full example program source code at: https://github.com/eddeighton/eg/tree/master/tests/reference/bbb_inheritance/aaa_concrete

Program Output:

+---------+-----+-------+--------+
|Timestamp|Type |Value  |Instance|
+=========+=====+=======+========+
|00000001 |START|root   |00000000|
+---------+-----+-------+--------+
|00000001 |START|root_A |00000000|
+---------+-----+-------+--------+
|00000001 |START|root_B |00000000|
+---------+-----+-------+--------+
|00000001 |START|root_C |00000000|
+---------+-----+-------+--------+
|00000001 |START|root_D |00000000|
+---------+-----+-------+--------+
|00000001 |START|root_E |00000000|
+---------+-----+-------+--------+
|00000001 |LOG  |A::test|        |
+---------+-----+-------+--------+
|00000001 |LOG  |B::test|        |
+---------+-----+-------+--------+
|00000001 |LOG  |C::test|        |
+---------+-----+-------+--------+
|00000001 |LOG  |D::test|        |
+---------+-----+-------+--------+
|00000001 |LOG  |E::test|        |
+---------+-----+-------+--------+
|00000001 |STOP |root   |00000000|
+---------+-----+-------+--------+
|00000001 |STOP |root_A |00000000|
+---------+-----+-------+--------+
|00000001 |STOP |root_B |00000000|
+---------+-----+-------+--------+
|00000001 |STOP |root_C |00000000|
+---------+-----+-------+--------+
|00000001 |STOP |root_D |00000000|
+---------+-----+-------+--------+
|00000001 |STOP |root_E |00000000|
+---------+-----+-------+--------+

