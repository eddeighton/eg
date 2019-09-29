


#include <gtest/gtest.h>

#include "eg/sessions/operations_session.hpp"

#include "test_utils.hpp"

#include "common/assert_verify.hpp"

#include <memory>

using DBPtr = std::unique_ptr< eg::OperationsSession >;

DBPtr loadDatabase( const char* pszTest )
{
    boost::filesystem::path testDir = 
        testutils::getEnvironmentVarPath( "EG_TESTS" );
    VERIFY_RTE_MSG( boost::filesystem::exists( testDir ), "Could not locate EG_TESTS environment variable" );
        
    boost::filesystem::path databaseFile =
        testutils::findFile( testDir / pszTest, "master.db" );
    VERIFY_RTE_MSG( boost::filesystem::exists( testDir ), "Could not locate test database: " << pszTest );
    
    return std::make_unique< eg::OperationsSession >( 
        databaseFile, boost::filesystem::path(), eg::IndexedObject::TU_FILES_BEGIN );
}


template< class T >
inline std::vector< eg::TypeID > toTypeIDs( const std::vector< T >& stuff )
{
    std::vector< eg::TypeID > result;
    for( const T& t : stuff )
        result.push_back( t->getIndex() );
    return result;
}
/*
TEST( NameResolution, LoadDatabase )
{
    DBPtr pDatabase = loadDatabase( "name_resolution_build" );
    const eg::IndexedObject::Array& objects = pDatabase->getObjects( eg::IndexedObject::MASTER_FILE );
    ASSERT_TRUE( !objects.empty() );
}
TEST( NameResolution, BasicName )
{
    DBPtr pDatabase = loadDatabase( "name_resolution_build" );
    
    // action root //14
    // {
    //   action A //15
    //   {
    //     action B //16
    //     {
    //       dim int x;//17
    //     }
    //   }
    //   action C : A //18
    //   {
    //     action D : B //19
    //   }
    //   link E : C::D //20
    //   {
    //     dim int x;//21
    //   }
    // }
    
    const std::vector< eg::TypeID > context = { 14 }; //root context
    const std::vector< eg::TypeID > implicitTypePath = { 15 }; //A
    eg::OperationID operationType = eg::id_Imp_NoParams; //start the action
    
    const eg::InvocationSolution::InvocationID invocationID = 
        eg::InvocationSolution::invocationIDFromTypeIDs( 
            pDatabase->getObjects( eg::IndexedObject::MASTER_FILE ), 
            pDatabase->getIdentifiers(),
            context, implicitTypePath, operationType );
    
    const eg::InvocationSolution* pInvocation = 
        pDatabase->getInvocation( invocationID, implicitTypePath );

    //ASSERT_EQ( toTypeIDs( pInvocation->getTargetTypes() ), std::vector< eg::TypeID >{ 15 } );
}*/