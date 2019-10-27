
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
                         " info: " << a.Info() <<
                         std::endl;
        }
        
    }
    
    template< typename T >
    void testInvoke( T actionRef )
    {
        //using TypePath = 
         //   __eg_type_path< __eg_root< void >::__eg_Test< void >, Start >;
        
        actionRef.foobar();
        
        //actionRef.template invoke< TypePath, Start >();
    }
    
}