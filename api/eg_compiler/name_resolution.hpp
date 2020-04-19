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

#include "common/requireSemicolon.hpp"

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

    class NameResolution;
            
    class Name
    {
        friend class NameResolution;
        
        InvocationSolution::ElementPair m_element;
        bool m_bIsMember;
        bool m_bIsReference;
        std::vector< std::size_t > m_children;
        
        Name( bool bIsMember, bool bIsReference )
            :   m_element( nullptr, nullptr ),
                m_bIsMember( bIsMember ),
                m_bIsReference( bIsReference )
        {
        }
        Name( InvocationSolution::ElementPair element, bool bIsMember, bool bIsReference )
            :   m_element( element ),
                m_bIsMember( bIsMember ),
                m_bIsReference( bIsReference )
        {
        }
    
        bool isMember() const { return m_bIsMember; }
    public:
        const interface::Element* getInterface() const { return m_element.first; }
        const concrete::Element* getConcrete() const { return m_element.second; }
        
        bool isReference() const { return m_bIsReference; }
        bool isTerminal() const { return m_children.empty(); }
        const std::vector< std::size_t >& getChildren() const { return m_children; }
    };
    
    class NameResolution
    {
        const DerivationAnalysis& m_analysis;
        const InvocationSolution::InvocationID& m_invocationID;
        std::vector< Name > m_nodes;
        
        //do not allow copy
        NameResolution( NameResolution& ) = delete;
        NameResolution& operator=( NameResolution& ) = delete;
    public:
        NameResolution( const DerivationAnalysis& analysis, 
            const InvocationSolution::InvocationID& invocationID,
            const InvocationSolution::ElementPairVector& context,
            const InvocationSolution::ElementPairVectorVector& typePath );
                    
        const std::vector< Name >& getNodes() const { return m_nodes; }
        
    private:
        Name* add( std::size_t iParent, InvocationSolution::ElementPair element, bool bIsMember, bool bIsReference );
        
        void expandReferences();
        
        void addType( const InvocationSolution::ElementPairVector& pathElement );
        
        void pruneBranches( Name* pNode );
        
        void resolve( const InvocationSolution::ElementPairVector& context,
                    const InvocationSolution::ElementPairVectorVector& typePath,
                    bool bExpandFinalReferences );
    };



}

#endif //NAME_RESOLUTION_05_05_2019