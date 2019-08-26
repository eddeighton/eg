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

#ifndef EG_RESOURCE_24_08_2019
#define EG_RESOURCE_24_08_2019

#include "common.hpp"

#include <string>
#include <memory>

namespace eg
{
    using ResourcePath = std::string;
        
    class Resource
    {
    public:
        using Ptr = std::shared_ptr< Resource >;
        virtual ~Resource();
    };

    class ResourceHandler
    {
    public:
        using Ptr = std::shared_ptr< ResourceHandler >;
        virtual ~ResourceHandler();
        virtual Resource::Ptr reload( const ResourcePath& path ) = 0;
    };
    
    //basic resource types
    class TextResource : public eg::Resource
    {
        friend class TextResourceHandler;
    public:
        const std::string& get() const { return m_str; }
        using Ptr = std::shared_ptr< TextResource >;
    private:
        std::string m_str;
    };

    class TextResourceHandler : public eg::ResourceHandler
    {
    protected:
        virtual eg::Resource::Ptr reload( const eg::ResourcePath& path );
    };


}

struct resource
{
private:
    static const eg::Resource::Ptr get_resource( const eg::ResourcePath& resourcePath );
    static void set_resource_handler( const eg::ResourcePath& resourcePath, eg::ResourceHandler::Ptr pHandler );
    
public:
    template< typename TResourceHandlerType >
    static void set( const eg::ResourcePath& resourcePath )
    {
        set_resource_handler( resourcePath, std::make_shared< TResourceHandlerType >() );
    }

    template< typename ResourceType >
    static std::shared_ptr< ResourceType > get( const eg::ResourcePath& resourcePath )
    {
        return std::dynamic_pointer_cast< ResourceType >( get_resource( resourcePath ) );
    }
};

#endif //EG_RESOURCE_24_08_2019