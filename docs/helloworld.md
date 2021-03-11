
---
layout: post
title: Hello World
---

# Hello World

The following would be a Hello World program in EG:

```
LOG( "Hello World" );
```

Perhaps a more elaborate example would be:

```
object World
{
    action SayHello
    {
        LOG( "Hello" );
    }
    action SayWorld
    {
        LOG( "World" );
    }
}

//start a world
World w = World();

//say hello
auto a = w.SayHello();

co_yield eg::wait( a );

//say world
w.SayWorld();

```

This program illustrates how an object can compose multiple actions which themselves are awaitable coroutines.


