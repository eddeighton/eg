---
title: EG documentation
layout: default
---

# {{ page.title }}

Welcome to the EG programming language documentation.

EG is a C++ extension language designed for creative programming and experimental gameplay development.

The EG language extends C++ by adding the ability to write .eg files.  Generally an eg program requires a host C++ program.  The current version supports a "basic" host and "cinder" host.  In the future it is hoped to support many hosts including all popular game engines.

A simple command line tool eg.exe is provided which facilitates creating, building and running eg projects.  An eg project must have a .eg file in its folder which tells the eg driver what host and packages to build.  Hosts and packages can load the eg program database and perform arbitrary code generation reflecting on the eg action tree.

When writing an EG source file you may no longer define C++ structures, classes or functions.  Instead you can only define actions.  Actions are the core concept of doing anything in EG.  One might say eg is an action oriented language.

# Actions

Generally an action is just like a C++ lambda except that its existence is addressable to the rest of the program.  If an action is active it may be enumerated, paused, stopped, read, written, waited on etc by any other action in the program.  

An eg program can be thought of as a simulation constituting a sequence of cycles.  Each cycle represents a point in time.  Every active action in the program must come to rest before the eg program will advance time to the next cycle.  This is achieved by either an action terminating or going to sleep by calling eg::sleep;

Actions do not have return values.  Instead an actions dimensions are able to be read by any other action.  If an action terminates its dimensions will still be able to be read by any other action and the actions memory is guarenteed not to be reclaimed for the remainder of the cycle.

## So what is an Action?

To define or declare an action use the `action` keyword i.e.

    ```c
    action foobar; //can just declare one
    
    action foo::bar; //can declare to any depth
    
    action foo
    {
        action bar
        {
            //still just a declaration because no actual code is defined
        }
    }
    
    //in square brackets the allocation size for the action may be defined with any compile time integer expression
    //actions use a lock free ring buffer to allocate within their bound.  The default allocation is one.  
    //Every time the action is invoked the allocator will allocate from the allocation.  
    //If there is no free space and error is generated to the event log and a null reference is returned.
    action foo[ 12 * 3 + 456 ];
    
    //actions may also have a parameter list
    action bar( std::shared_ptr< Thing > pParameter ); 
    ```c
    
## The Action Tree

So actions are basically like functions combined with classes.

The first interesting thing about eg is that actions are recursive.  Actions are defined in a tree i.e.

    ```c++

    //the root is actually an action itself
    std::cout << "Can put code into the root in any .eg file" << std::endl;
    
    action A
    {
        action B
        {
            action C
            {
                //actions all the way down...
            }
        }
    }
    ```
    
Actions can also inherit other actions with multiple inheritance. 


    ```c++
    abstract A //abstract is the same as action except the action only exists when inherited
    {
        //abstract cannot have code but can have nested structure
        action Nested
        {
            x(); //read x even though it does not exist here
        }
    }
    
    action B : A
    {
        Nested n = Nested();
        dim int x; //x is read by Nested
    }
    
    action C : B
    {
        action Nested
        {
            //override Nested from the A::Nested
        }
    }
    ```
Generally in eg all symbols are effectively virtual and override to the most deriving thing.  This is zero cost because everything is compile time polymorphism.
    
All .eg source files are merged into a single tree.

Using C++ template tricks it never matters what order actions are defined.  Any action can always address any other action irrespective of where it occurs in the tree.

    ```c++
    
    action Foobar
    {
        //includes are added through an include keyword
        //all includes are aggregated into a single include.hpp file which then gets precompiled into include.pch
        include( <vector> );
        include( "somewhere/mySourceCode.hpp" );
    
        DoSomethingDefinedLater.Start(); //this is fine
    }
    
    action DoSomethingDefinedLater
    {
        Foobar(); //infinite recursion is not fine - everything has a bounded stack in eg.
    }
    ```
    

# Dimensions

Variables within an action may be marked as a dimension which makes them addressable to the rest of the program.

    ```c++
    action Foo
    {
        float y; //normal local variable cannot be seen outside the action
        dim float x;
        dim std::vector< LaserTank > tanks;
        dim std::set< Foo > setOfFoobarRefs;
        dim Foo dimensionReference;
    }
    
    action Bar
    {
        float f = Foo.x(); //read x
        Foo.x( f + 1.0f ); //write x
        std::vector< LaserTank >& tanks = Foo.tanks.Get(); //get a reference
        
        Foo.dimensionReference.x(); //read through a reference
        Foo f = Foo.dimensionReference(); //read the reference itself
        f.dimensionReference.dimensionReference.dimensionReference();//etc
    }
    ```
    
## Dimension Traits

The way dimensions are handled can be customised by specialising DimensionTraits<> for the type.  

Generally all dimensions are preallocated before the program starts.

It is perfectly valid to address dimensions on an action that is not started and infact the .Raw operation is specifically for doing this which can be very useful for initialising data.

## Object Mappings

eg also allows inheriting from C++ types through the object mapping system.  This is primarily to enable integrating with third party tree structures like scene graphs and physics engines.  This is an important feature but unfortunately is not implemented yet.

# Traversals

So now we have a bounded tree of actions with dimensions.  The next feature of eg to understand is the traversal or type path.

Instead of using ordinary C++ name resolution and member expressions eg converts all names into types and then expresses all member calls as a type path.

This sounds complicated but it is pretty simple imagine the following:

    ```c++
    
    someReferenceToAnAction.A.B.C.Operation( parameters );
    
    ```
    
This can be understood instead as

    ```c++
    reference.invoke< someResultType, type_path< A, B, C >, Operation >( parameters );
    ```

This is all done transparently behind the scenes but what it means is:

- eg can convert all forms of structural, heterogenous polymorphism to compile time code
- eg has great prospects for interesting derivation semantics
- eg can in the future implement a fluent query language in the traversal system
- In the future eg programs will be able to be converted to pure standard C++ and compiled with any compiler.

This idea has been one of the central motivations to create eg and explore the possibilities of this idea.

## How does name resolution work

The simplest way to understand the name resolution may sound scary at first but hopefully it will make sense.

Take every action and dimension symbol in the entire program and put them into a set.
Any use of any of those symbols in the place of a temporary constructor expression of member call expression on an eg reference causes the compiler to recognise the expression as an invocation.

Sequential symbols in the type path can be used similarly to a C++ elaborated type specified to disambiguate the symbol from others i.e.

    ```c++
    action A
    {
        action B
    }
    
    action C
    {
        action B
    }
    
    B(); //ambiguous - no way to choose between A::B or C::B
    
    A.B(); //unambiguous
    ```
    
The compiler will also disambiguate via the derivation semantics i.e.
    
    ```c++
    action A[ 2 ]
    {
        action B
    }
    
    action C[ 1 ]
    {
        action B
    }
    
    B(); //This is NOT ambiguous because A::B cannot be derived from here because A is not singular so C::B is choosen
    
    ```
    
## How does derivation work

In short the most deriving thing is always choosen and this includes when invoking i.e.

    ```c++
    action A
    {
        action C;
        C(); // if the instance of A is B then C() will actually invoke D - abstract factory pattern...
    }
    
    action B : A
    {
        action D : A::C;
    }
    ```

TODO

# Compile Time Polymorphism

TODO

## Variants are first class

Everything you can do with an eg action reference you can do with a Variant of references.

## Links and metamorphic structure

Links are another really important feature of eg but are currently not implemented.
Links effectively are inbetween inheritance and a normal reference.  Essentially dynamic inheritance.
Links have the same derivation semantics as inheritance but the dynamic flexibility of a reference.

TODO

# Invocations

So the core system in the eg language is the invocation system.  An invocation is just like a member function call and can be a this call or on a reference.  The eg invocation system supports the following set of operations.  Operations are either supports on actions or dimensions or both.

## Operation Types

### Implicit

    ```c++
    
    action( parameters ); // will invoke the action syncronously
    dimension(); //will read the dimension
    dimension( value ); //will write to the dimension
    
    ```

### Start

    ```c++
    action.Start( parameters ); //will start the action in its own fiber
    ```
    
### Stop

    ```c++
    action.Stop(); //will stop the action fiber.  This means when the action fiber sleeps it will then stop
    ```
    This currently uses exceptions in order to work.
    
### Pause

    ```c++
    action.Pause(); //will pause the action once the action sleeps.
    ```
    Not currently implemented.
    
### Resume

    ```c++
    action.Resume(); //will resume a paused action.
    ```
    Not currently implemented.
    
### Wait

    ```c++
    action.Wait(); //will cause the calling fiber to wait until the action has gone to sleep
    dimension.Wait(); //will cause the calling fiber to wait until the dimension has been updated.
    ```
    Not currently implemented.
    
### Get

    ```c++
    action.Get(); //just gets a reference to the action 
    dimension.Get(); //gets a mutable reference to the data
    ```
    
### Update

    ```c++
    dimension.Update(); //signals a dimension is updated without modifying it.  For when something calls .Wait on the dimension
    ```
    Not currently implemented.
 
### Range

    ```c++
    for( Tanks t : Somewhere.Tanks.Range() )
    {
        t.DoStuff.Start();
    }
    
    Ranges can be polymorphic and heterogenous i.e.
    for( Var< Tank, Helicopter, Battleship > v : Vehicles.Range() )
    {
        v.doStuff(); //doStuff must exist on Tank and Helicopter and Battleship in some way...
    }
    ```

### Raw

    ```c++
    for( Tanks t : Somewhere.Tanks.Raw() )
    {
        t.x( 123 );//initialise x for all tanks
    }
    ```
    Allows access to all data irrespective of whether actions are running.
    Useful when using invocation system in python to initialise stuff.

### Done

    ```c++
    if( !Done() )
    {
        eg::sleep();
    }
    ```
    Simple way to check if an action has any active child actions. 
    The above code is a simple way to get a parent action to remain active while it has active child actions.
    
# The Execution Model

eg uses boost::fibers to implement user land threading.  Currently everything is single threaded.  Every time Start is called a new fiber is created.  Currently the fiber stack size is configured globally in the eg project file.  The default is 16K.  Having many thousands of fibers can quickly use up a lot of memory.  In the future combining fibers and coroutines may be the best approach.


## Waiting and Sleeping 

## eg::wait
    ```c++
    eg::wait(); //cause this fiber to be suspended
    
    //the fiber is guarenteed to be resume within the cycle unless the program terminates.
    ```
    Generally you would call eg::wait() because you are waiting for some other action to do something


## eg::sleep

    ```c++
    eg::sleep(); // fiber is now asleep - no more work this cycle
    
    Tank t = Tank.Get();
    
    t.Wait(); //wait until t has gone to sleep ( not currently implemented )
    
    eg::sleep( t ); //sleep until t stops.
    
    eg::sleep( [this]( Event e ){ return e == someAction || e == someOtherAction; } ); //pass in a resumption functor
    
    eg::sleep( 10ms ); //sleep for 10 milliseconds
    
    ```
    
# Python integration

eg automatically generates python bindings for the eg tree.  This can be turned off.
The entire invocation system can be used in python and the sleep and wait functions can also be used.
eg programs can be given any number of python scripts will all execute in their own fiber.

# The Event Log

All eg programs have an underlying event log which captures all events.  Generally the main event is an action stopping.  Message passing is ubiquitous in eg since one can easily sleep on any event and then read the dimensions of the stopped action.

    ```c++
    action Msg
    {
        dim std::string str;
    }
    
    action Sender
    {
        Msg().str( "Hello World" );
    }
    
    action Receiver
    {
        eg::sleep( []( Event e ) { return Msg( e ); ); //just see if event is a Msg by converting the Event to Msg type
        std::string& s = Msg.Get().str(); //get the string
    }
    ```
Event is a type erased reference type.  You cannot invoke on an event but any variant or reference can be converted to or from an event.
The eg type system is completely type safe and has compile time checks for all polymorphic type conversions including to and from variants.

# Host integration

eg currently only has two host

## basic

The basic host is primarily for testing purposes

## cinder

The cinder host provides a cinder and imgui integration which is great fun for programming particle systems and creative experiments but may not be suitable for more serious gameplay development.

In the future more hosts and packages will hopefully be supported.

