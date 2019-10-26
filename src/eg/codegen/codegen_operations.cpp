

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

#include "../eg.hpp"
#include "../input.hpp"
#include "../interface.hpp"

#include "../derivation.hpp"

#include <boost/bind.hpp>
#include <boost/algorithm/string.hpp>

#include <sstream>
#include <cctype>


namespace eg
{

    struct ExportSourceVisitor
    {
        std::ostream& os;
        const eg::TranslationUnit& m_translationUnit;
        std::string strIndent;

        ExportSourceVisitor( std::ostream& os, const eg::TranslationUnit& translationUnit )
            :   os( os ),
                m_translationUnit( translationUnit )
        {
        }

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
        void push ( const input::Export*   pElement, const interface::Element* pNode )
        {
        }
        void push ( const input::Root*      pElement, const interface::Element* pNode )
        {
            push( (input::Action*) pElement, pNode );
        }
        void push ( const input::Action*    pElement, const interface::Element* pNode )
        {
            const interface::Action* pAction = dynamic_cast< const interface::Action* >( pNode );
            VERIFY_RTE( pAction );

            //calculate the path to the root type
            std::vector< const interface::Element* > path = getPath( pNode );

            
            std::vector< interface::Export* > exports;
            pAction->getExports( exports );
            for( interface::Export* pExport : exports )
            {
                //generate type comment
                {
                    os << "\n//";
                    for( const interface::Element* pNodeIter : path )
                    {
                        if( pNodeIter != *path.begin())
                            os << "::";
                        os << pNodeIter->getIdentifier();
                    }
                    os << "::foobar\n";
                }

                //generate the template argument lists
                {
                    int iCounter = 1;
                    for( const interface::Element* pNodeIter : path )
                    {
                        //os << strIndent << "template< typename _EG" << iCounter << " >\n";
                        os << strIndent << "template<>\n";
                        ++iCounter;
                    }
                }

                //os << strIndent << "template< typename... Args >\n";

                //just generate an explicit template specialisation
                os << strIndent << "inline auto ";
                {
                    int iCounter = 1;
                    for( const interface::Element* pNodeIter : path )
                    {
                        //os << getInterfaceType( pNodeIter->getIdentifier() ) << "< _EG" << iCounter << " >::";
                        os << getInterfaceType( pNodeIter->getIdentifier() ) << "< void >::";
                        ++iCounter;
                    }

                    {
                        os << pExport->getIdentifier() << "() const\n";
                    }
                }

                //generate the function body
                os << strIndent << "{\n";
                //os << strIndent << "  __eg_root< void > r = *this;\n";
                os << strIndent << "  return " << pExport->getType() << "();\n";
                os << strIndent << "}\n";
            }
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
        void pop ( const input::Export*   pElement, const interface::Element* pNode )
        {
        }
        void pop ( const input::Root*      pElement, const interface::Element* pNode )
        {
        }
        void pop ( const input::Action*    pElement, const interface::Element* pNode )
        {
        }
    };

    struct OperationsSourceVisitor
    {
        std::ostream& os;
        const eg::TranslationUnit& m_translationUnit;
        std::string strIndent;

        OperationsSourceVisitor( std::ostream& os, const eg::TranslationUnit& translationUnit )
            :   os( os ),
                m_translationUnit( translationUnit )
        {
        }

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
        void push ( const input::Export*   pElement, const interface::Element* pNode )
        {
        }
        void push ( const input::Root*      pElement, const interface::Element* pNode )
        {
            push( (input::Action*) pElement, pNode );
        }
        void push ( const input::Action*    pElement, const interface::Element* pNode )
        {
            const interface::Action* pAction = dynamic_cast< const interface::Action* >( pNode );
            VERIFY_RTE( pAction );

            if( m_translationUnit.isAction( pAction ) )
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
        void pop ( const input::Export*   pElement, const interface::Element* pNode )
        {
        }
        void pop ( const input::Root*      pElement, const interface::Element* pNode )
        {
        }
        void pop ( const input::Action*    pElement, const interface::Element* pNode )
        {
        }
    };

    void generateOperationSource( std::ostream& os, const interface::Root* pRoot, const eg::TranslationUnit& translationUnit )
    {
        generateIncludeGuard( os, "OPERATIONS" );

        //generate exports first
        {
            ExportSourceVisitor visitor( os, translationUnit );
            pRoot->pushpop( visitor );
        }

        //generate operations
        {
            OperationsSourceVisitor visitor( os, translationUnit );
            pRoot->pushpop( visitor );
        }

        os << "\n" << pszLine << pszLine;
        os << "#endif\n";
    }


}

