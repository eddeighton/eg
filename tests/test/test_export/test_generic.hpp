
#include <algorithm>

namespace generic
{

    template< typename T >
    void printSize( const T& range )
    {
        std::cout << "Size: " << 
            std::distance( range.begin(), range.end() ) << std::endl;
    }
    
    
    template< typename T >
    void printRangeInfo( const T& range )
    {
        for( auto a : range )
        {
            std::cout << "Instance: " << a.data.instance <<
                         " Type: " << a.data.type <<
                         " timestamp: " << a.data.timestamp <<
                         std::endl;
        }
        
    }
    
    template< typename T >
    void testInvoke( T actionRef )
    {
        std::cout << "Test Invoke: " << actionRef.X() << std::endl;
        //actionRef.RunThing();
        
        for( auto i : actionRef.Things() )
        {
            i.Y( 123.0f );
        }
        
    }
    
    template< typename T >
    void printThings( T actionRef )
    {
        for( auto a : actionRef.Things() )
        {
            std::cout << "Instance: " << a.data.instance <<
                         " Type: " << a.data.type <<
                         " timestamp: " << a.data.timestamp <<
                         " y: " << a.Y() <<
                         std::endl;
        } 
    }
    
    template< typename T >
    struct GenericThing
    {
        std::vector< T > data;
        
        void DoStuff()
        {
            for( T i : data )
            {
                i.Go();
                
            }
        }
    };
}