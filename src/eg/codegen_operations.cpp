

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


#include "codegen.hpp"

#include "eg.hpp"
#include "input.hpp"
#include "interface.hpp"


#include <boost/bind.hpp>
#include <boost/algorithm/string.hpp>

#include <sstream>
#include <cctype>


namespace eg
{


    struct OperationsSourceVisitor
    {
        std::ostream& os;
        std::string strIndent;
        
        OperationsSourceVisitor( std::ostream& os ) : os( os ) {}
        
        
        void push ( const input::Opaque*    pElement, const interface::Element* pNode )
        {    
        }      
        void push ( const input::Dimension* pElement, const interface::Element* pNode )
        {    
        }    
        void push ( const input::Include*   pElement, const interface::Element* pNode )
        {    
        }      
        void push ( const input::Using*   pElement, const interface::Element* pNode )
        {    
        }  
        void push ( const input::Root*      pElement, const interface::Element* pNode )
        {    
            push( (input::Action*) pElement, pNode );
        }    
        void push ( const input::Action*    pElement, const interface::Element* pNode )
        {
            //calculate the path to the root type
            std::vector< const interface::Element* > path = getPath( pNode );
            
            //generate type comment
            {
                os << "\n//";
                for( const interface::Element* pNodeIter : path )
                {
                    if( pNodeIter != *path.begin())
                        os << "::";
                    os << pNodeIter->getIdentifier();
                }
                os << "\n";
            }
            
            //generate the template argument lists
            {
                int iCounter = 1;
                for( const interface::Element* pNodeIter : path )
                {
                    os << strIndent << "template<>\n";
                    ++iCounter;
                }
            }
            
            //just generate an explicit template specialisation
            os << strIndent << "void ";
            {
                for( const interface::Element* pNodeIter : path )
                {
                    os << getInterfaceType( pNodeIter->getIdentifier() ) << "< void >::";
                }
                if( const input::Opaque* pParams = pElement->getParams() )
                {
                    os << "operator()(" << pParams->getStr() << ") const\n";
                }
                else
                {
                    os << "operator()() const\n";
                }
            }
            
            //generate the function body
            os << strIndent << "{\n";
            strIndent.push_back( ' ' );
            strIndent.push_back( ' ' );
            
            for( const interface::Element* pChild : pNode->getChildren() )
            {
                if( const input::Opaque* pOpaque = 
                        dynamic_cast< const input::Opaque* >( pChild->getInputElement() ) )
                {
                    os << strIndent << pOpaque->getStr() << "\n";
                }
            }
            
            strIndent.pop_back();
            strIndent.pop_back();
            os << strIndent << "}\n";
        }
        void pop ( const input::Opaque*    pElement, const interface::Element* pNode )
        {    
        }      
        void pop ( const input::Dimension* pElement, const interface::Element* pNode )
        {    
        }    
        void pop ( const input::Include*   pElement, const interface::Element* pNode )
        {    
        }     
        void pop ( const input::Using*   pElement, const interface::Element* pNode )
        {    
        }   
        void pop ( const input::Root*      pElement, const interface::Element* pNode )
        {    
        }    
        void pop ( const input::Action*    pElement, const interface::Element* pNode )
        {
        }
    };
    
    void generateOperationSource( std::ostream& os, const interface::Root* pRoot )
    {
        generateIncludeGuard( os, "OPERATIONS" );
        
        //generate operations
        {
            OperationsSourceVisitor visitor( os );
            pRoot->pushpop( visitor );
        }
        
        os << "\n" << pszLine << pszLine;
        os << "#endif\n";
    }    
    
    
}

