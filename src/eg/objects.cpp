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


#include "eg_compiler/objects.hpp"
#include "eg_compiler/input.hpp"
#include "eg_compiler/interface.hpp"
#include "eg_compiler/allocator.hpp"
#include "eg_compiler/concrete.hpp"
#include "eg_compiler/translation_unit.hpp"
#include "eg_compiler/layout.hpp"
#include "eg_compiler/identifiers.hpp"
#include "eg_compiler/derivation.hpp"
#include "eg_compiler/link.hpp"
#include "eg_compiler/invocation.hpp"
#include "eg_compiler/instruction.hpp"

namespace eg
{
    IndexedObject* ObjectFactoryImpl::create( const IndexedObject& object )
    {
        IndexedObject* pObject = nullptr;

        switch( object.getType() )
        {
            case eInputOpaque                : pObject = new input::Opaque                  ( object );  break;
            case eInputDimension             : pObject = new input::Dimension               ( object );  break;
            case eInputInclude               : pObject = new input::Include                 ( object );  break;
            case eInputUsing                 : pObject = new input::Using                   ( object );  break;
            case eInputExport                : pObject = new input::Export                  ( object );  break;
            case eInputVisibility            : pObject = new input::Visibility              ( object );  break;
            case eInputContext               : pObject = new input::Context                 ( object );  break;
            case eInputRoot                  : pObject = new input::Root                    ( object );  break;
                                                                                           
            case eAbstractOpaque             : pObject = new interface::Opaque              ( object );  break;
            case eAbstractDimension          : pObject = new interface::Dimension           ( object );  break;
            case eAbstractInclude            : pObject = new interface::Include             ( object );  break;
            case eAbstractUsing              : pObject = new interface::Using               ( object );  break;
            case eAbstractExport             : pObject = new interface::Export              ( object );  break;
            case eAbstractAbstract           : pObject = new interface::Abstract            ( object );  break;
            case eAbstractEvent              : pObject = new interface::Event               ( object );  break;
            case eAbstractFunction           : pObject = new interface::Function            ( object );  break;
            case eAbstractAction             : pObject = new interface::Action              ( object );  break;
            case eAbstractObject             : pObject = new interface::Object              ( object );  break;
            case eAbstractLink               : pObject = new interface::Link                ( object );  break;
            case eAbstractRoot               : pObject = new interface::Root                ( object );  break;
                                
            case eInheritanceNode            : pObject = new concrete::Inheritance_Node     ( object );  break;
                                                                                            
            case eConcreteAction             : pObject = new concrete::Action               ( object );  break;
            case eConcreteDimensionUser      : pObject = new concrete::Dimension_User       ( object );  break;
            case eConcreteDimensionGenerated : pObject = new concrete::Dimension_Generated  ( object );  break;
            case eConcreteAllocator_Nothing  : pObject = new concrete::NothingAllocator     ( object );  break;
            case eConcreteAllocator_Singleton: pObject = new concrete::SingletonAllocator   ( object );  break;
            case eConcreteAllocator_Range    : pObject = new concrete::RangeAllocator       ( object );  break;
                                                                                            
            case eIdentifiers                : pObject = new Identifiers                    ( object );  break;
            case eDerivationAnalysis         : pObject = new DerivationAnalysis             ( object );  break;
            case eLinkGroup                  : pObject = new LinkGroup             	        ( object );  break;
            case eLinkAnalysis               : pObject = new LinkAnalysis             	    ( object );  break;
            case eTranslationUnit            : pObject = new TranslationUnit                ( object );  break;
            case eTranslationUnitAnalysis    : pObject = new TranslationUnitAnalysis        ( object );  break;
            case eInvocationSolution         : pObject = new InvocationSolution             ( object );  break;
                                                                                            
            case eDataMember                 : pObject = new DataMember                     ( object );  break;
            case eBuffer                     : pObject = new Buffer                         ( object );  break;
            case eLayout                     : pObject = new Layout                         ( object );  break;

            case TOTAL_OBJECT_TYPES:
            default:
                THROW_RTE( "Unknown object type" );
                break;
        }
        return pObject;
    }


}