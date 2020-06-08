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

#include "generators.hpp"

#include "eg_compiler/sessions/implementation_session.hpp"
#include "eg_compiler/interface.hpp"
#include "eg_compiler/codegen/codegen.hpp"

#include "common/assert_verify.hpp"
#include "common/file.hpp"

#include <boost/program_options.hpp>

#include <iostream>

int main( int argc, const char* argv[] )
{
    const eg::CmdLine cmdLine = eg::handleCommandLine( argc, argv );
    
    eg::ReadSession session( cmdLine.programDataBaseFile );
    
    std::ostringstream os;
    
    const eg::interface::Root* pRoot = session.getTreeRoot();
    
    const eg::DerivationAnalysis& derivationAnalysis = session.getDerivationAnalysis();
    const eg::Layout& layout = session.getLayout();
    const eg::IndexedObject::Array& objects = session.getObjects( eg::IndexedObject::MASTER_FILE );
    
    os << "#include \"structures.hpp\"\n";
	
os << "#ifdef DEBUG\n";
os << "		#include <iostream>\n";
os << "#endif\n";
        
    os << "\n//buffers\n";
    for( const eg::Buffer* pBuffer : layout.getBuffers() )
    {
        os << "static std::unique_ptr< std::array< " << pBuffer->getTypeName() << ", " << pBuffer->getSize() << " > > " << 
            pBuffer->getVariableName() << "_allocation;\n";
        os << pBuffer->getTypeName() << "* " << pBuffer->getVariableName() << " = nullptr;\n";
    }
    
    os << "void allocate_buffers()\n";
    os << "{\n";
    for( const eg::Buffer* pBuffer : layout.getBuffers() )
    {
    os << "    " << pBuffer->getVariableName() << "_allocation = std::make_unique< std::array< " << 
        pBuffer->getTypeName() << ", " << pBuffer->getSize() << " > >();\n";
    os << "    " << pBuffer->getVariableName() << " = " << pBuffer->getVariableName() << "_allocation->data();\n";
    
    os << "    for( " << eg::EG_INSTANCE << " i = 0U; i != " << pBuffer->getSize() << "; ++i )\n";
    os << "    {\n";
        for( const eg::DataMember* pDimension : pBuffer->getDimensions() )
        {
            eg::generateAllocation( os, pDimension, "i" );
        }
    
    os << "    }\n";
    }
    os << "}\n";
    
    os << "\n";
    
    os << "void deallocate_buffers()\n";
    os << "{\n";
    //deallocate in reverse
    for( std::size_t sz = layout.getBuffers().size(); sz > 0U; --sz )
    {
        const eg::Buffer* pBuffer = layout.getBuffers()[ sz - 1U ];
    os << "    for( " << eg::EG_INSTANCE << " i = 0U; i != " << pBuffer->getSize() << "; ++i )\n";
    os << "    {\n";
        for( const eg::DataMember* pDimension : pBuffer->getDimensions() )
        {
            eg::generateDeallocation( os, pDimension, "i" );
        }
    os << "    }\n";
    }
    os << "}\n";
    os << "\n";
    
    os << "\n";
    os << "//Action functions\n";
    os << "extern __eg_root< void > root_starter();\n";
    os << "extern void root_stopper( eg::Instance _gid );\n";
    os << "\n";
    
    const char* pszMainRoutine = R"(
    
int main( int argc, const char* argv[] )
{
    int iMilliseconds = 0;
    
    try
    {
		
#ifdef DEBUG
		{
			char c;
			std::cin >> c;
		}
#endif

        //const float sleepDuration_sec = 
        //    std::chrono::duration< float, std::ratio< 1 > >(
        //        std::chrono::milliseconds( iMilliseconds ) ).count();
        
        //allocate everything
        allocate_buffers();
        
        eg::Scheduler::start( root_starter(), &root_stopper );
        
        float cycleStart = clock::ct();
        while( eg::Scheduler::active() )
        {
            //const float elapsed = clock::ct() - cycleStart;
            //if( elapsed < sleepDuration_sec )
            //{
            //    auto floatDuration      = std::chrono::duration< float, std::ratio< 1 > >( sleepDuration_sec - elapsed );
            //    auto intMilliseconds    = std::chrono::duration_cast< std::chrono::milliseconds >( floatDuration );
            //    std::this_thread::sleep_for( intMilliseconds );
            //}
            
            eg::Scheduler::cycle();
            
            clock::next();
            
            cycleStart = clock::ct();
        }
        
        deallocate_buffers();
    }
    catch( std::exception& e )
    {
        deallocate_buffers();
        return 1;
    }
    
    return 0;
    
}
    
    )";
    os << pszMainRoutine << "\n";
    
    boost::filesystem::updateFileIfChanged( cmdLine.strBuildDir / std::string( "main.cpp" ), os.str() );    

    return 0;
}