
#ifndef EG_LOG_30_OCT_2020
#define EG_LOG_30_OCT_2020

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/daily_file_sink.h"
#include "spdlog/async.h"

#include "common/processID.hpp"

#include "boost/filesystem.hpp"

#include <chrono>
#include <sstream>

namespace eg
{
    inline auto configureLog( const boost::filesystem::path& logFolderPath, const std::string& strLogName )
    {
        spdlog::drop( strLogName );
        
        if( logFolderPath.empty() )
        {
            auto console_sink = std::make_shared< spdlog::sinks::stdout_color_sink_mt >();
            {
                console_sink->set_level( spdlog::level::info );
                console_sink->set_pattern( "%l [%^%l%$] %v");
            }
            
            auto threadPool = std::make_shared< spdlog::details::thread_pool >( 8192, 1 );
            auto logger = std::shared_ptr< spdlog::async_logger >( 
                new spdlog::async_logger( strLogName, { console_sink }, 
                    threadPool, spdlog::async_overflow_policy::block ) );
            {
                logger->set_level( spdlog::level::trace );
            }
            
            //spdlog::flush_every( std::chrono::seconds( 1 ) );
            spdlog::set_default_logger( logger );
            
            return threadPool;
        }
        else
        {
            auto console_sink = std::make_shared< spdlog::sinks::stdout_color_sink_mt >();
            {
                console_sink->set_level( spdlog::level::info );
                console_sink->set_pattern( "%l [%^%l%$] %v");
            }
            
            std::ostringstream osLogFileName;
            osLogFileName << strLogName << "_" << Common::getProcessID() << ".log";
            
            const boost::filesystem::path logFilePath = logFolderPath / osLogFileName.str();
            
            auto file_sink = std::make_shared< spdlog::sinks::daily_file_sink_st >( logFilePath.string(), 23, 59 );
            {
                file_sink->set_level( spdlog::level::trace );
            }
            auto threadPool = std::make_shared< spdlog::details::thread_pool >( 8192, 1 );
            auto logger = std::shared_ptr< spdlog::async_logger >( 
                new spdlog::async_logger( strLogName, {console_sink, file_sink}, 
                    threadPool, spdlog::async_overflow_policy::block ) );
            {
                logger->set_level( spdlog::level::trace );
            }
            
            spdlog::flush_every( std::chrono::seconds( 1 ) );
            spdlog::set_default_logger( logger );
            
            return threadPool;
        }
    }

}

#endif //EG_LOG_30_OCT_2020