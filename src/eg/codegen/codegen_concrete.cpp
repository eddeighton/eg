

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


#include "eg_compiler/codegen/codegen.hpp"

#include "eg_compiler/eg.hpp"
#include "eg_compiler/input.hpp"
#include "eg_compiler/interface.hpp"
#include "eg_compiler/concrete.hpp"
#include "eg_compiler/derivation.hpp"
#include "eg_compiler/layout.hpp"
#include "eg_compiler/identifiers.hpp"
#include "eg_compiler/derivation.hpp"
#include "eg_compiler/translation_unit.hpp"
#include "eg_compiler/invocation.hpp"

#include <boost/algorithm/string.hpp>

#include <sstream>
#include <cctype>

namespace eg
{
/*
    */
    
    
    void generateDataMemberType( std::ostream& os, const concrete::Dimension_User* pDimension )
    {
        const interface::Dimension* pNodeDimension = 
            dynamic_cast< const interface::Dimension* >( pDimension->getAbstractElement() );
        if( pNodeDimension->getActionTypes().empty() )
        {
            os << pNodeDimension->getCanonicalType();
        }
        else if( pNodeDimension->getActionTypes().size() == 1U )
        {
            const interface::Action* pAction = pNodeDimension->getActionTypes().front();
            
            os << getStaticType( pAction );
        }
        else
        {
            os << EG_VARIANT_TYPE << "< ";
            for( const interface::Action* pAction : pNodeDimension->getActionTypes() )
            {
                if( pAction != pNodeDimension->getActionTypes().front() )
                    os << ", ";
                os << getStaticType( pAction );
            }
            os << " >";
        }
    }
    
    void generateDataMemberType( std::ostream& os, const concrete::Dimension_Generated* pDimension )
    {
        switch( pDimension->getDimensionType() )
        {
            //case eDimensionTimestamp :
            //    os << EG_TIME_STAMP;
            //    break;
            case concrete::Dimension_Generated::eActionStopCycle    :
                os << EG_TIME_STAMP;
                break;
            case concrete::Dimension_Generated::eActionState        :
                os << EG_ACTION_STATE;
                break;
            case concrete::Dimension_Generated::eActionReference    :
                {
                    const concrete::Action* pDimensionAction = pDimension->getAction();
                    VERIFY_RTE( pDimensionAction );
                    const interface::Action* pAction = pDimensionAction->getAction();
                    os << getStaticType( pAction );
                }
                break;
            case concrete::Dimension_Generated::eActionAllocatorData:
                os << EG_INSTANCE;
                break;
            case concrete::Dimension_Generated::eActionAllocatorHead:
                os << EG_ALLOCATOR_TYPE;
                break;
            case concrete::Dimension_Generated::eRingIndex:
                os << EG_INSTANCE;
                break;
			case concrete::Dimension_Generated::eLinkReference:
                os << EG_REFERENCE_TYPE;
                break;
            default:
                THROW_RTE( "Unknown generated dimension type" );
        }
    }
    
    void generateDataMemberAllocation( std::ostream& os, const concrete::Dimension_User* pDimension, Printer& printer )
    {
        static const std::string strIndent = "        ";
        
        //use the traits
        const interface::Dimension* pNodeDimension = dynamic_cast< const interface::Dimension* >( pDimension->getAbstractElement() );
        if( pNodeDimension->getActionTypes().empty() )
        {
            os << strIndent << "::eg::DimensionTraits< " << pNodeDimension->getCanonicalType() << " >::initialise( " << printer << " );\n";
        }
    }
    
    void generateDataMemberAllocation( std::ostream& os, const concrete::Dimension_Generated* pDimension, Printer& printer )
    {
        static const std::string strIndent = "        ";
        switch( pDimension->getDimensionType() )
        {
            case concrete::Dimension_Generated::eActionStopCycle   :
                {
                    os << strIndent << printer << " = " << EG_INVALID_TIMESTAMP << ";\n";
                }
                break;
            case concrete::Dimension_Generated::eActionState       :
                {
                    os << strIndent << printer << " = " << getActionState( action_stopped ) << ";\n";
                }
                break;
            case concrete::Dimension_Generated::eActionReference       :
                {
                    const concrete::Action* pDimensionAction = pDimension->getAction();
                    VERIFY_RTE( pDimensionAction );
                    const interface::Action* pAction = pDimensionAction->getAction();
                    os << strIndent << printer << " = " << getStaticType( pAction ) << 
                        "( " << EG_REFERENCE_TYPE << " { i, " << pDimensionAction->getIndex() << ", 0 } );\n";
                }
                break;
            case concrete::Dimension_Generated::eActionAllocatorData   : os << strIndent << printer << " = i;\n";   		break;
            case concrete::Dimension_Generated::eActionAllocatorHead   : os << strIndent << printer << " = 0UL;\n"; 		break;
            case concrete::Dimension_Generated::eRingIndex             : os << strIndent << printer << " = i;\n";   		break;
			case concrete::Dimension_Generated::eLinkReference         : os << strIndent << printer << " = { 0, 0, 0 };\n"; break;
            default:
                THROW_RTE( "Unknown generated dimension type" );
        }
    }
    
    void generateDataMemberDeallocation( std::ostream& os, const concrete::Dimension_User* pDimension, Printer& printer )
    {
        
    }
    
    void generateDataMemberDeallocation( std::ostream& os, const concrete::Dimension_Generated* pDimension, Printer& printer )
    {
        
    }
    /*
    
    void Dimension_User::printStart( std::ostream& os, const IPrintDimensions& printer, const std::string& strIndex ) const
    {
    }
    void Dimension_User::printStop( std::ostream& os, const IPrintDimensions& printer, const std::string& strIndex ) const
    {
    }
	void Dimension_User::printEncode( std::ostream& os, const IPrintDimensions& printer, const std::string& strIndex ) const
	{
        static const std::string strIndent = "        ";
        
        //use the traits
        const interface::Dimension* pNodeDimension = dynamic_cast< const interface::Dimension* >( m_pElement );
        if( pNodeDimension->getActionTypes().empty() )
        {
            os << strIndent << "::eg::DimensionTraits< " << pNodeDimension->getCanonicalType() << " >::encode( buffer, ";
            printer.printVariableAccess( os, strIndex );
            os << ");";
        }
	}
	void Dimension_User::printDecode( std::ostream& os, const IPrintDimensions& printer, const std::string& strIndex ) const
	{
        static const std::string strIndent = "        ";
        
        //use the traits
        const interface::Dimension* pNodeDimension = dynamic_cast< const interface::Dimension* >( m_pElement );
        if( pNodeDimension->getActionTypes().empty() )
        {
            os << strIndent << "::eg::DimensionTraits< " << pNodeDimension->getCanonicalType() << " >::decode( buffer, ";
            printer.printVariableAccess( os, strIndex );
            os << ");";
        }
	}
            
	void Dimension_Generated::printEncode( std::ostream& os, const IPrintDimensions& printer, const std::string& strIndex ) const
	{
        static const std::string strIndent = "        ";
        
		os << strIndent << "::eg::DimensionTraits< ";
		
        switch( m_type )
        {
            //case eDimensionTimestamp :
            //    os << EG_TIME_STAMP;
            //    break;
            case eActionStopCycle    :
                os << EG_TIME_STAMP;
                break;
            case eActionState        :
                os << EG_ACTION_STATE;
                break;
            case eActionReference    :
                {
                    VERIFY_RTE( m_pAction );
                    const interface::Action* pAction = m_pAction->getAction();
                    os << getStaticType( pAction );
                }
                break;
            case eActionAllocatorData:
                os << EG_INSTANCE;
                break;
            case eActionAllocatorHead:
                os << EG_ALLOCATOR_TYPE;
                break;
            case eRingIndex:
                os << EG_INSTANCE;
                break;
			case concrete::eLinkReference: 
            default:
                THROW_RTE( "Unknown generated dimension type" );
        }
		
		os << " >::encode( buffer, ";
		printer.printVariableAccess( os, strIndex );
		os << ");";
	}
	void Dimension_Generated::printDecode( std::ostream& os, const IPrintDimensions& printer, const std::string& strIndex ) const
	{
        static const std::string strIndent = "        ";
        
		os << strIndent << "::eg::DimensionTraits< ";
		
        switch( m_type )
        {
            //case eDimensionTimestamp :
            //    os << EG_TIME_STAMP;
            //    break;
            case eActionStopCycle    :
                os << EG_TIME_STAMP;
                break;
            case eActionState        :
                os << EG_ACTION_STATE;
                break;
            case eActionReference    :
                {
                    VERIFY_RTE( m_pAction );
                    const interface::Action* pAction = m_pAction->getAction();
                    os << getStaticType( pAction );
                }
                break;
            case eActionAllocatorData:
                os << EG_INSTANCE;
                break;
            case eActionAllocatorHead:
                os << EG_ALLOCATOR_TYPE;
                break;
            case eRingIndex:
                os << EG_INSTANCE;
                break;
            default:
                THROW_RTE( "Unknown generated dimension type" );
        }
		
		os << " >::decode( buffer, ";
		printer.printVariableAccess( os, strIndex );
		os << ");";
	}
    
    
    
    
    
    
    
    
    
    
    
    void Action::printType( std::ostream& os ) const
    {
        os << getStaticType( getAction() );
    }
	void Action::printEncode( std::ostream& os, const std::string& strIndex ) const
	{
		const std::vector< std::string >& params = getAction()->getParameters();
		//TODO - generate dimensions for parameters for defered / remote calls
	}
	void Action::printDecode( std::ostream& os, const std::string& strIndex ) const
	{
		const std::vector< std::string >& params = getAction()->getParameters();
		//TODO - generate dimensions for parameters for defered / remote calls
	}
    
    
    
    
        
    void DataMember::print( std::ostream& os ) const
    {
        m_pDimension->printType( os );
    }


    void DataMember::printVariableAccess( std::ostream& os, const std::string& strIndex ) const
    {
        os << getBuffer()->getVariableName() << "[ " << strIndex << " ]." << getName();
    }

    void DataMember::printDependencyVar( std::ostream& os, const std::string& strIndex ) const
    {
        if( m_pDependency )
        {
            const concrete::Dimension_Generated* pGen = 
                dynamic_cast< const concrete::Dimension_Generated* >( m_pDimension );
            
            os << m_pDependency->getBuffer()->getVariableName() << 
                "[ " << strIndex << " / " << pGen->getDependencyDomain() << " ]." << m_pDependency->getName();
        }
    }

    void DataMember::printAllocation( std::ostream& os, const std::string& strIndex ) const
    {
        return m_pDimension->printAllocation( os, *this, strIndex );
    }
    void DataMember::printDeallocation( std::ostream& os, const std::string& strIndex ) const
    {
        return m_pDimension->printDeallocation( os, *this, strIndex );
    }
    void DataMember::printStart( std::ostream& os, const std::string& strIndex ) const
    {
        return m_pDimension->printStart( os, *this, strIndex );
    }
    void DataMember::printStop( std::ostream& os, const std::string& strIndex ) const
    {
        return m_pDimension->printStop( os, *this, strIndex );
    }

    void DataMember::printEncode( std::ostream& os, const std::string& strIndex ) const
    {
        return m_pDimension->printEncode( os, *this, strIndex );
    }
    void DataMember::printDecode( std::ostream& os, const std::string& strIndex ) const
    {
        return m_pDimension->printDecode( os, *this, strIndex );
    }

*/


    std::ostream& operator<<( std::ostream& os, const Printer& printer )
    {
        os << printer.m_pDataMember->getBuffer()->getVariableName() << "[ " << 
            printer.pszIndex << " ]." << printer.m_pDataMember->getName();
        return os;
    }
    
    void generateDataMemberType( std::ostream& os, const DataMember* pDataMember )
    {
        const concrete::Dimension* pDimension = pDataMember->getInstanceDimension();
        switch( pDimension->getType() )
        {
            case eConcreteDimensionUser:
                generateDataMemberType( os, dynamic_cast< const concrete::Dimension_User* >( pDimension ) );
                break;
    
            case eConcreteDimensionGenerated:
                generateDataMemberType( os, dynamic_cast< const concrete::Dimension_Generated* >( pDimension ) );
                break;
            default:
                THROW_RTE( "Invalid dimension type" );
        }
    }
    
    void generateAllocation( std::ostream& os, const DataMember* pDataMember, const std::string& strIndex )
    {
        const concrete::Dimension* pDimension = pDataMember->getInstanceDimension();
        Printer printer( pDataMember, strIndex.c_str() );
        switch( pDimension->getType() )
        {
            case eConcreteDimensionUser:
                generateDataMemberAllocation( os, dynamic_cast< const concrete::Dimension_User* >( pDimension ), printer );
                break;
    
            case eConcreteDimensionGenerated:
                generateDataMemberAllocation( os, dynamic_cast< const concrete::Dimension_Generated* >( pDimension ), printer );
                break;
            default:
                THROW_RTE( "Invalid dimension type" );
        }
    }
    
    void generateDeallocation( std::ostream& os, const DataMember* pDataMember, const std::string& strIndex )
    {
        const concrete::Dimension* pDimension = pDataMember->getInstanceDimension();
        Printer printer( pDataMember, strIndex.c_str() );
        switch( pDimension->getType() )
        {
            case eConcreteDimensionUser:
                generateDataMemberDeallocation( os, dynamic_cast< const concrete::Dimension_User* >( pDimension ), printer );
                break;
    
            case eConcreteDimensionGenerated:
                generateDataMemberDeallocation( os, dynamic_cast< const concrete::Dimension_Generated* >( pDimension ), printer );
                break;
            default:
                THROW_RTE( "Invalid dimension type" );
        }
    }
}