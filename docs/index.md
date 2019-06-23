---
title: eg documentation
layout: default
---

# {{ page.title }}

Welcome to the eg programming language documentation.

eg is a c++ extension language designed for creative programming and experimental gameplay development.

The eg language extends C++ by adding the ability to write .eg files.  Generally an eg program requires a host C++ program.  The current version supports a "basic" host and "cinder" host.  In the future it is hoped to support many hosts including all popular game engines.

When writing an eg source file you may no longer define C++ structures, classes or functions.  Instead you can only define actions.  Actions are the core concept of eg combined with dimensions.

# Actions

Generally an action is just like a C++ function except that its existence is addressable to the rest of the program.  If an action is active it may be read and written by any other action.  A critical thing to understand with eg programming is how the program executes as a simulation with cycles.  Every cycle every action must come to rest by going to sleep.  If an action is stopped it will always be addressable by other actions for the rest of that cycle.

## So what is an Action?

To define or declare an action use the `action` keyword i.e.

    ```c++
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
    action foo[ allocationSize ];
    
    //actions may also have a parameter list
    action bar( std::shared_ptr< Thing > pParameter ); 
    ```
    
    
## The Action Tree

So actions are basically like functions pretty simple so far...

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
    
All .eg source files are merged into a single tree which becomes the eg program interface.

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
    }
    ```
    
## Dimension Traits

The way dimensions are handled can be customised by specialising DimensioTraits<> for the type.  

Generally all dimensions are preallocated before the program starts.

It is perfectly valid to address dimensions on an action that is not started and infact the .Raw operation is specifically for doing this which can be very useful for initialising data.

## Object Mappings

eg also allows inheriting from C++ types through the object mapping system.  This is primarily to enable integrating with third party tree structures like scene graphs and physics engines.

Not currently implemented.

# Traversals

So now we have a bounded tree of actions with dimensions.  The next feature of eg to understand is the traversal or type path.

Instead of using ordinary C++ name resolution and member expressions eg converts all names into types and then expresses all member or this calls as a type path.

This sounds complicated but it is pretty simple imagine the following:

    ```c++
    
    someReferenceToAnAction.A.B.C.Operation( parameters );
    
    ```
    
This can be understood instead as

    ```c++
    
    reference.invoke< type_path< A, B, C >, Operation >( parameters );
    
    ```
This is all done transparently behind the scenes but what it means is:

- eg can convert all forms of structural, heterogenous polymorphism to compile time code
- eg can solve at compile time any kind of derivation as long as it is unambiguous
- eg can effectively implement a path algebra ( query language ) over the action tree 
- The entire program can ultimately be converted to standard C++
    
This idea has been one of the central motivations to create eg and explore the possibilities of this approach to how member function calls can work.

## How does name resolution work

TODO

## How does derivation work

TODO

# Compile Time Polymorphism

TODO

## Everything is virtual

TODO

## Variants are first class

TODO

## Derivation works structurally

TODO

## Links and metamorphic structure

TODO

# Invocations

## Graceful conversion to C++ via result_type

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

eg uses boost::fibers to implement user land threading.  Currently everything is single threads but you can have thousands of fibers.

The idea is to think in terms of cycles.  Every cycle every fiber must go to sleep.  Once all fibers are asleep the scheduler will start the next cycle.

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
    
    eg::sleep( 10ms ); //sleep for 10 milliseconds
    
    ```
    
# Python integration

eg automatically generates python bindings for the eg tree.  This can be turned off.
The entire invocation system can be used in python and the sleep and wait functions can also be used.
eg programs can be given any number of python scripts will all execute in their own fiber.

# Host integration

eg currently only has two host

## basic

The basic host is primarily for testing purposes

## cinder

The cinder host provides a cinder and imgui integration which is great fun for programming particle systems and creative experiments but may not be suitable for more serious gameplay development.

In the future more hosts and packages will hopefully be supported.

