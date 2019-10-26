
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
        std::cout << "Test Invoke: " << actionRef.GetX() << std::endl;
        actionRef.RunThing();
    }
    
}