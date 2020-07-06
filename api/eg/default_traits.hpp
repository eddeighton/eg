//  Copyright (c) Deighton Systems Limited. 2019. All Rights Reserved.
//  Author: Edward Deighton
//  License: Please see license.txt in the project root folder.

//  Use and copying of this software and preparation of derivative works
//  based upon this software are permitted. Any copy of this software or
//  of any derivative work must include the above copyright notice, this
//  paragraph and the one after it.  Any distribution of this software or
//  derivative works must comply with all applicable laws.

//  This software is made available AS IS, and COPYRIGHT OWNERS DISCLAIMS
//  ALL WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
//  PURPOSE, AND NOTWITHSTANDING ANY OTHER PROVISION CONTAINED HEREIN, ANY
//  LIABILITY FOR DAMAGES RESULTING FROM THE SOFTWARE OR ITS USE IS
//  EXPRESSLY DISCLAIMED, WHETHER ARISING IN CONTRACT, TORT (INCLUDING
//  NEGLIGENCE) OR STRICT LIABILITY, EVEN IF COPYRIGHT OWNERS ARE ADVISED
//  OF THE POSSIBILITY OF SUCH DAMAGES.


#ifndef EG_DEFAULT_TRAITS
#define EG_DEFAULT_TRAITS

#include <cstddef>
#include <type_traits>

namespace eg
{

//default dimension traits 
template< typename T >
struct DimensionTraits
{
    using Read  = const T&;
    using Write = T;
    using Get   = T&;
    static const std::size_t Size = sizeof( T );
    static const std::size_t Simple = std::is_trivially_copyable< T >::value;
    
    static void initialise( T& value )
    {
        new (&value ) T;
    }
    static void uninitialise( T& value )
    {
        value.~T();
    }
};

}

#endif //EG_DEFAULT_TRAITS