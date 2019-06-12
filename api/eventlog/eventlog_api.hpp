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

#ifndef EVENT_LOG_MODULE_API_01_05_2019
#define EVENT_LOG_MODULE_API_01_05_2019

#ifdef EG_EVENTLOG_SHARED_MODULE
    #define EGEVENTLOG_EXPORT __declspec( dllexport )
#else
    #define EGEVENTLOG_EXPORT __declspec( dllimport )
#endif

#include <cstddef>
#include <cstdint>

namespace eg
{
    class EGEVENTLOG_EXPORT EventLogServer
    {
    public:
        virtual ~EventLogServer();
        
        virtual std::uint64_t head() = 0;
        virtual bool updateHead() = 0;
        
        virtual void write( const char* pszType, std::size_t szTypeSize, std::size_t timestamp, 
                const void* pValue, std::size_t szValueSize ) = 0;
                
        virtual bool read( std::uint64_t& iterator, const char*& pszType, std::size_t& timestamp, 
                const void*& pValue, std::size_t& szValueSize ) = 0;
                
        static EventLogServer* create( const char* pszFilePath );
        static EventLogServer* create( const char* pszPID, const char* pszFilePath );
    };

    class EGEVENTLOG_EXPORT EventLogClient
    {
    public:
        virtual ~EventLogClient();
        
        virtual std::uint64_t head() = 0;
                
        virtual bool read( std::uint64_t& iterator, const char*& pszType, std::size_t& timestamp, 
                const void*& pValue, std::size_t& szValueSize ) = 0;
        
        static EventLogClient* create( const char* pszPID, const char* pszFilePath );
    };

}

#endif //EVENT_LOG_MODULE_API_01_05_2019
