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
        function test() : int 
        {
            return 1;
        }
    }
    action B : A
    {
        dim int y;
        function test() : int 
        {
            return 2;
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
        function test() : int 
        {
            return 3;
        }
    }
    action D : C
    {
        //has domain size of four
        function test() : int 
        {
            return 4;
        }
    };
    action E[ 5 ] : C
    {
        //has domain size of five
        function test() : int 
        {
            return 5;
        }
    };

