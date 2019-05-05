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


#ifndef NAME_RESOLUTION_05_05_2019
#define NAME_RESOLUTION_05_05_2019

#include "derivation.hpp"
#include "invocation.hpp"

#include "common/require_semicolon.hpp"

#include <exception>

namespace eg
{
    class NameResolutionException : public std::runtime_error
    {
    public:
        NameResolutionException( const std::string& strMessage )
            :   std::runtime_error( strMessage )
        {}
    };
    
    #define THROW_NAMERESOLUTION_EXCEPTION( msg ) \
        DO_STUFF_AND_REQUIRE_SEMI_COLON( \
            std::ostringstream _os; \
            _os << msg; \
            throw NameResolutionException( _os.str() );)

    struct Name
    {
        const concrete::Inheritance_Node* pInheritanceNode    = nullptr;
        const concrete::Dimension*        pDimension          = nullptr;
        int score;
        bool isTarget;
        std::vector< int > children;
        
        Name( const concrete::Inheritance_Node* pInheritanceNode, int score, bool isTarget )
            :   pInheritanceNode( pInheritanceNode ),
                score( score ),
                isTarget( isTarget )
        {
        }
        Name( const concrete::Dimension* pDimension, int score, bool isTarget )
            :   pDimension( pDimension ),
                score( score ),
                isTarget( isTarget )
        {
        }
        
        const concrete::Element* getInstance() const
        {
            if( pInheritanceNode )
                return pInheritanceNode->getRootConcreteAction();
            else if( pDimension )
                return pDimension;
            else
            {
                THROW_RTE( "Invalid name" );
            }
        }
    };

    
    class NameResolution
    {
        const DerivationAnalysis& m_analysis;
        
        //do not allow copy
        NameResolution( NameResolution& ) = delete;
        NameResolution& operator=( NameResolution& ) = delete;
    public:
        NameResolution( const DerivationAnalysis& analysis, const InvocationSolution::InvocationID& invocationID );
        
        const std::vector< Name >& getNodes() const { return nodes; }
        
    private:
        Name* add( int iParent, const concrete::Inheritance_Node* pInheritanceNode, int score, bool bIsTarget = false );
        Name* add( int iParent, const concrete::Dimension* pDimension, int score, bool bIsTarget = false );
        
        std::vector< Name > nodes;
        
        void expandReferences();
        
        void addType( const std::vector< const concrete::Element* >& instances );
        
        void pruneBranches( Name* pNode );
        
        void resolve( 
            const std::vector< const concrete::Inheritance_Node* >& contextInstances,
            const std::vector< std::vector< const concrete::Element* > >& typePathInstances );
    };



}

#endif //NAME_RESOLUTION_05_05_2019