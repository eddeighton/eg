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

//#include "runtime/eg_coroutine.hpp"

#include "common/file.hpp"
#include "common/assert_verify.hpp"

#include <boost/program_options.hpp>
#include <boost/fiber/all.hpp>
#include <boost/bind.hpp>

#include "pybind11/embed.h"
#include "pybind11/numpy.h"
#include "pybind11/stl.h"
#include "pybind11/stl_bind.h"

#include <iostream>
#include <memory>
#include <array>
#include <thread>
#include <chrono>
#include <functional>
#include <chrono>



int m_currentCycle = 0;

void incrementCycle()
{
    ++m_currentCycle;
}

class eg_props : public boost::fibers::fiber_properties 
{
public:
    eg_props( boost::fibers::context* pContext )
        :   fiber_properties( pContext ),
            m_cycle( m_currentCycle ) ,
            m_terminate( false )
    {
    }

    int getCycle() const 
    {
        return m_cycle;
    }

    // Call this method to alter priority, because we must notify
    // priority_scheduler of any change.
    void setCycle( int iCycle ) 
    { 
        if( m_cycle != iCycle )
        {
            m_cycle = iCycle;
            //notify();
        }
    }
    bool isTerminated() const { return m_terminate; }
    
    void terminate()
    {
        if( !m_terminate )
        {
            m_terminate = true;
            //notify();
        }
    }
    
    std::string strName;
    
private:
    int m_cycle;
    bool m_terminate;
};

std::set< boost::fibers::fiber::id > blockedWaitingFibers;
    
struct eg_algorithm : public boost::fibers::algo::algorithm_with_properties< eg_props >
{
    typedef boost::fibers::scheduler::ready_queue_type rqueue_t;
    
    rqueue_t m_queue;
    
    std::mutex                  mtx_{};
    std::condition_variable     cnd_{};
    bool                        flag_{ false };
    

    eg_algorithm() 
        :   m_queue()
    {
    }
    
    ~eg_algorithm()
    {
    }
    
    const eg_props& getContextProps( const boost::fibers::context* pContext ) const noexcept
    {
        return const_cast< eg_algorithm* >( this )->properties( 
            const_cast< boost::fibers::context* >( pContext ) );
    }

    virtual void awakened( boost::fibers::context* pContext, eg_props& props ) noexcept
    {
        if( !getContextProps( pContext ).strName.empty() )
        {
            blockedWaitingFibers.erase( pContext->get_id() );
            std::cout << "awakened: " << getContextProps( pContext ).strName << std::endl;
        }
        m_queue.push_back( *pContext );
    }

    virtual boost::fibers::context* pick_next() noexcept
    {
        rqueue_t::iterator iTimeKeeperIter = m_queue.end();
        
        for( rqueue_t::iterator 
            i = m_queue.begin(),
            iEnd = m_queue.end(); i!=iEnd; ++i )
        {
            boost::fibers::context* pContext( &*i );
            if( properties( pContext ).getCycle() <= m_currentCycle )
            {
                if( getContextProps( pContext ).strName == "timeKeeper" )
                {
                    iTimeKeeperIter = i;
                }
                else
                {
                    m_queue.erase( i );
                    if( !getContextProps( pContext ).strName.empty() )
                        std::cout << "pick_next: " << getContextProps( pContext ).strName << std::endl;
                    return pContext;
                }
            }
        }
        if( iTimeKeeperIter != m_queue.end() )
        {
            if( !blockedWaitingFibers.empty() )
            {
                //error
                std::cout << "error waiting fibers are blocked but not fibers can be resumed" << std::endl;
            }
            boost::fibers::context* pContext( &*iTimeKeeperIter );
            m_queue.erase( iTimeKeeperIter );
            return pContext;
        }
        else
        {
            return nullptr;
        }
    }

    virtual bool has_ready_fibers() const noexcept
    {
        for( rqueue_t::const_iterator 
            i = m_queue.begin(),
            iEnd = m_queue.end(); i!=iEnd; ++i )
        {
            const boost::fibers::context* pContext( &*i );
            
            if( getContextProps( pContext ).getCycle() <= m_currentCycle )
            {
                return true;
            }
        }
        return false;
    }
    
    virtual void property_change( boost::fibers::context * ctx, eg_props & props ) noexcept 
    {
        // Although our priority_props class defines multiple properties, only
        // one of them (priority) actually calls notify() when changed. The
        // point of a property_change() override is to reshuffle the ready
        // queue according to the updated priority value.
//<-
        //std::cout << "property_change(" << props.name << '(' << props.get_priority()
        //          << ")): ";
//->

        // 'ctx' might not be in our queue at all, if caller is changing the
        // priority of (say) the running fiber. If it's not there, no need to
        // move it: we'll handle it next time it hits awakened().
        if ( !ctx->ready_is_linked() ) 
        { /*<
            Your `property_change()` override must be able to
            handle the case in which the passed `ctx` is not in
            your ready queue. It might be running, or it might be
            blocked. >*/
//<-
            // hopefully user will distinguish this case by noticing that
            // the fiber with which we were called does not appear in the
            // ready queue at all
            //describe_ready_queue();
//->
            return;
        }

        // Found ctx: unlink it
        ctx->ready_unlink();

        // Here we know that ctx was in our ready queue, but we've unlinked
        // it. We happen to have a method that will (re-)add a context* to the
        // right place in the ready queue.
        awakened( ctx, props);
    }

    virtual void suspend_until( std::chrono::steady_clock::time_point const& timePoint ) noexcept
    {
        //VERIFY_RTE( !has_ready_fibers() );
        if ( (std::chrono::steady_clock::time_point::max)() == timePoint ) 
        {
            std::unique_lock< std::mutex > lk( mtx_ );
            cnd_.wait( lk, [this](){ return flag_; } );
            flag_ = false;
            std::cout << ".";
        } 
        else 
        {
            std::unique_lock< std::mutex > lk( mtx_ );
            cnd_.wait_until( lk, timePoint, [this](){ return flag_; } );
            
            //using namespace std::chrono_literals;
            //cnd_.wait_until( lk, std::chrono::steady_clock::now() + 1s, [this](){ return flag_; } );
            
            flag_ = false;
            //std::cout << "suspend_until: " << m_currentCycle << std::endl;
        }
    }

    virtual void notify() noexcept
    {
        //std::cout << "notify called: " << m_currentCycle << std::endl;
        
        std::unique_lock< std::mutex > lk( mtx_ );
        flag_ = true;
        lk.unlock();
        cnd_.notify_all();
    }
};

void timeKeeper()
{
    using namespace std::chrono_literals;
    
    for( int i = 0; i < 5; ++i )
    {
        boost::this_fiber::sleep_until( std::chrono::steady_clock::now() + 100ms );
        incrementCycle();
        blockedWaitingFibers.clear();
        std::cout << "cycle: " << m_currentCycle << std::endl;
    }
    
}

boost::fibers::fiber fiberOne, fiberTwo, fiberThree, fiberTerminated;

void wait()
{
    std::cout << "wait: " << boost::this_fiber::properties< eg_props >().strName << std::endl;
    boost::this_fiber::yield();
}
/*
void wait( boost::fibers::fiber& fiber )
{
    std::cout << "wait: " << boost::this_fiber::properties< eg_props >().strName << std::endl;
    blockedWaitingFibers.insert( boost::this_fiber::get_id() );
    fiber.join();
}*/

void sleep()
{
    std::cout << "sleep: " << boost::this_fiber::properties< eg_props >().strName << std::endl;
    boost::this_fiber::properties< eg_props >().setCycle( m_currentCycle + 1 );
    boost::this_fiber::yield();
}
/*
void sleep( boost::fibers::fiber& fiber )
{
    std::cout << "sleep: " << boost::this_fiber::properties< eg_props >().strName << std::endl;
    boost::this_fiber::properties< eg_props >().setCycle( m_currentCycle + 1 );
    fiber.join();
}*/


PYBIND11_EMBEDDED_MODULE( pyeg, module ) 
{
    module.def( "sleep", sleep );
    module.def( "wait", wait );
}


void runPython( const std::string& strScriptFile )
{
    if( !strScriptFile.empty() )
    {
        std::string strScriptText;
        {
            const boost::filesystem::path pythonFilePath = 
                boost::filesystem::edsCannonicalise(
                    boost::filesystem::absolute( strScriptFile ) );
            if( !boost::filesystem::exists( pythonFilePath ) )
            {
                std::cout << "Cannot locate file: " << pythonFilePath.string() << std::endl;
                return;
            } 
            boost::filesystem::loadAsciiFile( pythonFilePath, strScriptText );
        }
        if( !strScriptText.empty() )
        {

            try
            {
                pybind11::module pyeg_module = pybind11::module::import( "pyeg" );

                pybind11::exec( strScriptText );
            }
            catch( std::exception& e )
            {
                std::cout << e.what() << std::endl;
            }
        }
    }
}


void fiberCount( int count )
{
    for( int i = 0; i < count; ++i )
    {
        wait();
        
        sleep();
    }
}

void fiberWait( boost::fibers::fiber& fiberToWaitOn )
{
    sleep();
    
    sleep();
        
    //wait( fiberToWaitOn );
    
    sleep();
}

void fiberTerminate()
{
    sleep();
    
    sleep();
    
    boost::this_fiber::properties< eg_props >().terminate();
    
    wait();
    
    if( boost::this_fiber::properties< eg_props >().isTerminated() )
        return;
    
    sleep();
    
    sleep();

}

int main( int argc, const char* argv[] )
{
    bool bDebug = false;
    std::vector< std::string > scripts;
    
    {
        namespace po = boost::program_options;
        po::variables_map vm;
        try
        {
            po::options_description desc("Allowed options");
            desc.add_options()
                ("help", "produce help message")
                
                //options
                ("debug",       po::value< bool >( &bDebug )->implicit_value( true ), 
                    "Wait at startup to allow attaching a debugger" )
                ("script",      po::value<  std::vector< std::string > >( &scripts ), "Python scripts" )
            ;

            po::positional_options_description p;
            p.add( "script", -1 );

            po::store( po::command_line_parser( argc, argv).
                        options( desc ).
                        positional( p ).run(),
                        vm );
            po::notify(vm);

            if (vm.count("help"))
            {
                std::cout << desc << "\n";
                return 1;
            }
        }
        catch( std::exception& )
        {
            std::cout << "Invalid input. Type '--help' for options\n";
            return 1;
        }
    }
    
    if( bDebug )
    {
        char c;
        std::cin >> c;
    }
    
    std::cout << sizeof( boost::fibers::fiber ) << std::endl;
    
    
    //std::cout << "Setting up" << std::endl;
    boost::fibers::use_scheduling_algorithm< eg_algorithm >();
    
    boost::this_fiber::properties< eg_props >().strName = "main";
    
    pybind11::scoped_interpreter guard{}; // start the interpreter and keep it alive
    
    //load python fibers
    {
        std::size_t szCount = 0;
        for( const std::string& strScript : scripts )
        {
            boost::fibers::fiber pythonFiber( boost::bind( &runPython, strScript ) );
            std::ostringstream os;
            os << "pythonFiber_" << szCount;
            ++szCount;
            pythonFiber.properties< eg_props >().strName = os.str();
            pythonFiber.detach();
        }
    }
    
    fiberOne = boost::fibers::fiber( boost::bind( &fiberCount, 2 ) );
    fiberOne.properties< eg_props >().strName = "fiberOne";
    
    //fiberTwo = boost::fibers::fiber( boost::bind( &fiberCount, 4 ) );
    //fiberTwo.properties< eg_props >().strName = "fiberTwo";
    
    fiberThree = boost::fibers::fiber( boost::bind( &fiberWait, boost::ref( fiberOne ) ) );
    fiberThree.properties< eg_props >().strName = "fiberWait";
    
    //fiberTwo = boost::fibers::fiber( boost::bind( &fiberWait, 1 ) );
    //fiberThree = boost::fibers::fiber( boost::bind( &fiberWait, 2 ) );
    //
    //fiberTerminated = boost::fibers::fiber( fiberTerminate );
    //
    boost::fibers::fiber fTimeKeeper( timeKeeper );
    fTimeKeeper.properties< eg_props >().strName = "timeKeeper";
    
    fTimeKeeper.join();
    
    if( fiberOne.joinable() )
        fiberOne.detach();
    
    if( fiberTwo.joinable() )
        fiberTwo.detach();
    
    if( fiberThree.joinable() )
        fiberThree.detach();
    
    //if( fiberTerminated.joinable() )
    //    fiberTerminated.detach();
    
    /*for( int i = 0; i != 5; ++i )
    {
        using namespace std::chrono_literals;
        std::this_thread::sleep_for( 1s );
    }*/
    
    //std::cout << "terminating" << std::endl;
    return 0;
}

