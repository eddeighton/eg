

#pragma once

#include <memory>

class Thing
{
public:
    using Ptr = std::shared_ptr< Thing >;
    int value = 0;
};

namespace eg
{
    template<>
    struct DimensionTraits< Thing::Ptr >
    {
        using Read  = Thing::Ptr;
        using Write = Thing::Ptr;
        using Get   = Thing::Ptr;
        static const std::size_t Size = sizeof( Thing::Ptr );
        static const std::size_t Simple = false;
        
        static void initialise( Thing::Ptr& value )
        {
            value = std::make_shared< Thing >();
        }
        static void uninitialise( Thing::Ptr& value )
        {
            //do nothing
        }
        
    };
}