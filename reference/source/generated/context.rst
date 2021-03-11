#######
Context
#######
**********
Allocation
**********
========
Singular
========

`Singular Program Source Code <https://github.com/eddeighton/eg/tree/master/tests/reference/aaa_context/ccc_allocation/aaa_singular/Singular.eg>`_

------
Simple
------

objects do not contain executable code themselves

+------------------------+------------+----------+----------+
| Header row, column 1   | Header 2   | Header 3 | Header 4 |
| (header rows optional) |            |          |          |
+========================+============+==========+==========+
| body row 1, column 1   | column 2   | column 3 | column 4 |
+------------------------+------------+----------+----------+
| body row 2             | ...        | ...      |          |
+------------------------+------------+----------+----------+



example::

    object SingularObject
    {
        //by default any context WITHOUT a size specification will be singular.
        //Being singular means the context can be automatically derived through when resolving names and dynamic contexts.
    };

--------
Advanced
--------
^^^^^^^^^^^
Equivalence
^^^^^^^^^^^

objects do not contain executable code themselves



example::

    action SingularObject::Nested
    {
        action DeeperNesting
        {
            action EvenDeeper
            {
                //Singular all the way down
                dim int x;
            }
        }
    }
    //because everything here is singular it is possible to directly address x from the root i.e.
    int i = x();
    //which translates as
    TEST( &x() == &SingularObject.Nested.DeeperNesting.EvenDeeper.x() ); //same address

Program Output:

+---------+-----+-----------------------------------------------------------+--------+
|Timestamp|Type |Value                                                      |Instance|
+=========+=====+===========================================================+========+
|00000001 |START|root                                                       |00000000|
+---------+-----+-----------------------------------------------------------+--------+
|00000001 |PASS |&x() == &SingularObject.Nested.DeeperNesting.EvenDeeper.x()|        |
+---------+-----+-----------------------------------------------------------+--------+
|00000001 |STOP |root                                                       |00000000|
+---------+-----+-----------------------------------------------------------+--------+

====
Many
====

`Many Program Source Code <https://github.com/eddeighton/eg/tree/master/tests/reference/aaa_context/ccc_allocation/bbb_many/Many.eg>`_

-----
Basic
-----

In order to define a non-singular allocation one must explicitly define a size for the type.  This size is always relative to the contexts parent so for every instance of the parent there will be that many instances of the type.

------------
Intermediate
------------

In order to define a non-singular allocation one must explicitly define a size for the type.  This size is always relative to the contexts parent so for every instance of the parent there will be that many instances of the type.


example::

    //define a parent with a domain size of three.  The context is relative to the root which is always singular.  So this means the total domain size is also three.
    action parent[ 3 ]
    {
        //by defining a child of domain size four every parent will have four children.  Therefore there would be a total of twelve children in the complete domain.
        action child[ 4 ]
        {
        }
    }
    LOG( "If the size is not specified it defaults to one which makes the context singular" );

Program Output:

+---------+-----+--------------------------------------------------------------------------------+--------+
|Timestamp|Type |Value                                                                           |Instance|
+=========+=====+================================================================================+========+
|00000001 |START|root                                                                            |00000000|
+---------+-----+--------------------------------------------------------------------------------+--------+
|00000001 |LOG  |If the size is not specified it defaults to one which makes the context singular|        |
+---------+-----+--------------------------------------------------------------------------------+--------+
|00000001 |STOP |root                                                                            |00000000|
+---------+-----+--------------------------------------------------------------------------------+--------+

