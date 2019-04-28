
#ifndef EG_DEFAULT_TRAITS
#define EG_DEFAULT_TRAITS

namespace eg
{

//default dimension traits 
template< typename T >
struct DimensionTraits
{
    using Read  = const T&;
    using Write = T;
    static const Instance Size = sizeof( T );
};

//default object traits
template< typename T >
struct ObjectTraits
{
    using Dependency = void;
    
    using PtrType = T*;
    
    static const Instance Size = sizeof( PtrType );
    
    static PtrType Allocate()
    {
        return new T;
    }
    static void Deallocate( PtrType p )
    {
        delete p;
    }
    static void Start( PtrType p )
    {
        //do nothing
    }
    static void Stop( PtrType p )
    {
        //do nothing
    }
    static void Pause( PtrType p )
    {
        //do nothing
    }
    static void Unpause( PtrType p )
    {
        //do nothing
    }
    
    //copy, restore, serialise, python bindings....
};  

}

#endif //EG_DEFAULT_TRAITS