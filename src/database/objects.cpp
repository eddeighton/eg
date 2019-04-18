
#include "objects.hpp"

#include "input.hpp"
#include "abstract.hpp"

namespace eg
{
    IndexedObject* ObjectFactoryImpl::create( const IndexedObject& object )
    {
        IndexedObject* pObject = nullptr;

        switch( object.getType() )
        {
            case eInputOpaque                    : pObject = new input::Opaque               ( object );  break;
            case eInputDimension                 : pObject = new input::Dimension            ( object );  break;
            case eInputInclude                   : pObject = new input::Include              ( object );  break;
            case eInputAction                    : pObject = new input::Action               ( object );  break;
            case eInputRoot                      : pObject = new input::Root                 ( object );  break;
                    
            case eAbstractOpaque                 : pObject = new abstract::Opaque            ( object );  break;
            case eAbstractDimension              : pObject = new abstract::Dimension         ( object );  break;
            case eAbstractInclude                : pObject = new abstract::Include           ( object );  break;
            case eAbstractAction                 : pObject = new abstract::Action            ( object );  break;
            case eAbstractRoot                   : pObject = new abstract::Root              ( object );  break;
                                
            /*case eInheritanceNode           : pObject = new Inheritance_Node            ( object );  break;
                                
            case eInstanceAction            : pObject = new Instance_Action             ( object );  break;
            case eInstanceDimensionUser     : pObject = new Instance_Dimension_User     ( object );  break;
            case eInstanceDimensionGenerated: pObject = new Instance_Dimension_Generated( object );  break;
        
            case eDerivationAnalysis        : pObject = new DerivationAnalysis          ( object );  break;
            case eInvocationSolution        : pObject = new InvocationSolution          ( object );  break;
                    
            case eDimensionInstance         : pObject = new DimensionInstance           ( object );  break;
            case eBuffer                    : pObject = new Buffer                      ( object );  break;
            case eLayout                    : pObject = new Layout                      ( object );  break;*/

            case TOTAL_OBJECT_TYPES:
            default:
                THROW_RTE( "Unknown object type" );
                break;
        }
        return pObject;
    }


}