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


#include <map>
#include <thread>
#include <functional>
#include <chrono>

namespace
{
    using namespace std::chrono_literals;

    class ResourceManager
    {
        struct ResourceTimeStamp
        {
            std::time_t                 timestamp;
            eg::Resource::Ptr           resource;
            eg::ResourceHandler::Ptr    handler;
        };
        
        using ResourceMap = std::map< eg::ResourcePath, ResourceTimeStamp >;
        static constexpr auto DELAY_TIME = 500ms;
        
        ResourceMap         m_resources;
        std::thread         m_thread;
        std::mutex          m_mutex;
        std::atomic< bool > m_bContinue = true;
        
    public:
        ResourceManager()
            :   m_thread( std::bind( &ResourceManager::thread_function, this ) )
        {
            
        }
        
        ~ResourceManager()
        {
            m_bContinue = false;
            m_thread.join();
        }
        
        const eg::Resource::Ptr get_resource( const eg::ResourcePath& resourcePath )
        {
            std::lock_guard lock( m_mutex );
            ResourceMap::const_iterator iFind = m_resources.find( resourcePath );
            if( iFind == m_resources.end() )
            {
                throw std::runtime_error( "Failed to locate resource: " + resourcePath );
            }
            return iFind->second.resource;
        }
        
        void set_resource_handler( const eg::ResourcePath& resourcePath, eg::ResourceHandler::Ptr pHandler )
        {
            std::lock_guard lock( m_mutex );
            
            ResourceMap::const_iterator iFind = m_resources.find( resourcePath );
            if( iFind != m_resources.end() )
            {
                throw std::runtime_error( "Resource manager already has handler for: " + resourcePath );
            }
            
            const std::time_t resource_timestamp = boost::filesystem::last_write_time( resourcePath );
            eg::Resource::Ptr pResource = pHandler->reload( resourcePath );
            
            m_resources[ resourcePath ] = ResourceTimeStamp{ resource_timestamp, pResource, pHandler };
            LOG( "Loaded resource: " << resourcePath );
        }
        
    private:
        void thread_function()
        {
            //VERY BAD NEWS - without the following pointless statement - some how this thread function is ELIMINATED by the compiler??
            std::this_thread::sleep_for( DELAY_TIME );
            
            while( m_bContinue )
            {
                {
                    std::lock_guard lock( m_mutex );
                
                    for( ResourceMap::iterator 
                        i = m_resources.begin(),
                        iEnd = m_resources.end(); i!=iEnd; ++i )
                    {
                        const eg::ResourcePath& resourcePath    = i->first;
                        ResourceTimeStamp& res                  = i->second;
                        const std::time_t resource_timestamp = 
                            boost::filesystem::last_write_time( resourcePath );
                        
                        if( res.timestamp != resource_timestamp )
                        {
                            //found a resource to reload...
                            res.timestamp = resource_timestamp;
                            res.resource  = res.handler->reload( resourcePath );
                            LOG( "Reloaded resource: " << resourcePath );
                            break;
                        }
                    }
                }
                std::this_thread::sleep_for( DELAY_TIME );
            }
        }
        
        
    };
    
    ResourceManager g_resource_manager;
    
    
}

namespace eg
{
    Resource::~Resource()
    {
    }
    
    ResourceHandler::~ResourceHandler()
    {
    }

    eg::Resource::Ptr TextResourceHandler::reload( const eg::ResourcePath& path )
    {
        const boost::filesystem::path filePath = path;
        std::ifstream inputFileStream( filePath.native().c_str(), std::ios::in );
        if( !inputFileStream.good() )
        {
            ERR( "Failed to open file: " << filePath.string() );
            throw std::runtime_error( "Failed to open fileL " + filePath.string() );
        }
        std::string fileContents( (std::istreambuf_iterator<char>( inputFileStream )),
                                   std::istreambuf_iterator<char>() );
                                   
        std::shared_ptr< TextResource > pResource = std::make_shared< TextResource >();
        pResource->m_str.swap( fileContents );
        return pResource;
    }

}

const eg::Resource::Ptr resource::get_resource( const eg::ResourcePath& resourcePath )
{
    return g_resource_manager.get_resource( resourcePath );
}

void resource::set_resource_handler( const eg::ResourcePath& resourcePath, eg::ResourceHandler::Ptr pHandler )
{
    g_resource_manager.set_resource_handler( resourcePath, pHandler );
}