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


#include "eg_compiler/codegen/dataAccessPrinter.hpp"
#include "eg_compiler/concrete.hpp"
#include "eg_compiler/layout.hpp"

namespace eg
{

    std::ostream& operator<<( std::ostream& os, const Printer& printer )
    {
        printer.print( os );
        return os;
    }
    
    struct DefaultPrinter : public Printer
    {
        DefaultPrinter( const DataMember* pDataMember, const char* pszIndex )
            :   m_pDataMember( pDataMember ),
                m_pszIndex( pszIndex)
        {
        }
    private:
        const DataMember* m_pDataMember;
        const char* m_pszIndex;
    protected:
        virtual void print( std::ostream& os ) const
        {
            os << m_pDataMember->getBuffer()->getVariableName() << 
                "[ " << m_pszIndex << " ]." << m_pDataMember->getName();
        }
    };
    
    struct DefaultPrinterFactory : public PrinterFactory
    {
        Printer::Ptr getPrinter( const DataMember* pDataMember, const char* pszIndex )
        {
            return std::make_shared< DefaultPrinter >( pDataMember, pszIndex );
        }
    };
    
    PrinterFactory::Ptr getDefaultPrinterFactory()
    {
        static PrinterFactory::Ptr g_default = 
            std::make_shared< DefaultPrinterFactory >();
        return g_default;
    }
}
