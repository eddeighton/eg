
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

#ifndef PRINTER_29_06_2020
#define PRINTER_29_06_2020

#include <ostream>
#include <memory>

namespace eg
{
    class DataMember;

    struct Printer
    {
        virtual ~Printer(){};
        using Ptr = std::shared_ptr< Printer >;
    private:
        Printer( Printer& ) = delete;
        Printer& operator=( Printer& ) = delete;
    protected:
        Printer(){}
        friend std::ostream& operator<<( std::ostream& os, const Printer& printer );
        virtual void print( std::ostream& os ) const = 0;
    };
    
    std::ostream& operator<<( std::ostream& os, const Printer& printer );
    
    struct PrinterFactory
    {
        virtual ~PrinterFactory(){};
        using Ptr = std::shared_ptr< PrinterFactory >;
    private:
        PrinterFactory( PrinterFactory& ) = delete;
        PrinterFactory& operator=( PrinterFactory& ) = delete;
    public:
        PrinterFactory(){}
        virtual Printer::Ptr getPrinter( const DataMember* pDataMember, const char* pszIndex ) = 0;
        virtual Printer::Ptr read( const DataMember* pDataMember, const char* pszIndex ) = 0;
        virtual Printer::Ptr write( const DataMember* pDataMember, const char* pszIndex ) = 0;
    };
    
    PrinterFactory::Ptr getDefaultPrinterFactory();

}

#endif //PRINTER_29_06_2020