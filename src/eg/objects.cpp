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


#include "objects.hpp"

#include "input.hpp"
#include "abstract.hpp"
#include "concrete.hpp"
#include "derivation.hpp"
#include "layout.hpp"
#include "identifiers.hpp"
#include "derivation.hpp"
#include "invocation.hpp"
#include "instruction.hpp"

namespace eg
{
    IndexedObject* ObjectFactoryImpl::create( const IndexedObject& object )
    {
        IndexedObject* pObject = nullptr;

        switch( object.getType() )
        {
            case eInputOpaque                : pObject = new input::Opaque                 ( object );  break;
            case eInputDimension             : pObject = new input::Dimension              ( object );  break;
            case eInputInclude               : pObject = new input::Include                ( object );  break;
            case eInputAction                : pObject = new input::Action                 ( object );  break;
            case eInputRoot                  : pObject = new input::Root                   ( object );  break;
                                                                                           
            case eAbstractOpaque             : pObject = new abstract::Opaque              ( object );  break;
            case eAbstractDimension          : pObject = new abstract::Dimension           ( object );  break;
            case eAbstractInclude            : pObject = new abstract::Include             ( object );  break;
            case eAbstractAction             : pObject = new abstract::Action              ( object );  break;
            case eAbstractRoot               : pObject = new abstract::Root                ( object );  break;
                                
            case eInheritanceNode            : pObject = new concrete::Inheritance_Node    ( object );  break;
                                     
            case eConcreteAction             : pObject = new concrete::Action              ( object );  break;
            case eConcreteDimensionUser      : pObject = new concrete::Dimension_User      ( object );  break;
            case eConcreteDimensionGenerated : pObject = new concrete::Dimension_Generated ( object );  break;
        
            case eIdentifiers                : pObject = new Identifiers                   ( object );  break;
            case eDerivationAnalysis         : pObject = new DerivationAnalysis            ( object );  break;
            case eInvocationSolution         : pObject = new InvocationSolution            ( object );  break;
                    
            case eDataMember                 : pObject = new DataMember                    ( object );  break;
            case eBuffer                     : pObject = new Buffer                        ( object );  break;
            case eLayout                     : pObject = new Layout                        ( object );  break;

            case TOTAL_OBJECT_TYPES:
            default:
                THROW_RTE( "Unknown object type" );
                break;
        }
        return pObject;
    }


}