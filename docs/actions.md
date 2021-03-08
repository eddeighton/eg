---
layout: page
title: Actions
order: 2
---

# Actions

An action is just like a C++ lambda except that its existence is addressable to the rest of the program.  If an action is active it may be enumerated, paused, stopped, read, written, waited on etc by any other action in the program.  

An eg program can be thought of as a simulation constituting a sequence of cycles.  Each cycle represents a point in time.  Every active action in the program must come to rest before the eg program will advance time to the next cycle.  This is achieved by either an action terminating or going to sleep by calling eg::sleep;

Actions do not have return values.  Instead an actions dimensions are able to be read by any other action.  If an action terminates its dimensions will still be able to be read by any other action and the actions memory is guarenteed not to be reclaimed for the remainder of the cycle.

## So what is an Action?

To define or declare an action use the `action` keyword i.e.

{% highlight ruby %}
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
{% endhighlight %}
    
## The Action Tree

So actions are basically like functions combined with classes.

The first interesting thing about eg is that actions are recursive.  Actions are defined in a tree i.e.

```c

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


```c
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
    
