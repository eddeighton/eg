
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

#ifndef INSRUCTION_CODE_GENERATOR_17_JULY_2020
#define INSRUCTION_CODE_GENERATOR_17_JULY_2020

#include <ostream>
#include <memory>

namespace eg
{
    class CodeGenerator
    {
        static const int INDENT = 4;
    public:
        CodeGenerator( const Layout& layout, int iIndent, const std::string& strFailureType, PrinterFactory& printerFactory ) 
            :   m_layout( layout ), 
                m_strFailureType( strFailureType ),
                m_printerFactory( printerFactory )
        {
            for( int i = 0; i != iIndent; ++i )
                pushIndent();
        }
        
        using VariableExprMap = std::map< const Variable*, std::string >;
        
        inline void setVarExpr( const Variable* pVariable, const std::string& str )
        {
            VariableExprMap::const_iterator iFind = m_variables.find( pVariable );
            VERIFY_RTE( iFind == m_variables.end() );
            m_variables.insert( std::make_pair( pVariable, str ) );
        }
        inline const std::string& getVarExpr( const Variable* pVariable ) const
        {
            VariableExprMap::const_iterator iFind = m_variables.find( pVariable );
            VERIFY_RTE( iFind != m_variables.end() );
            return iFind->second;
        }
        
        const std::string& getIndent() const { return m_strIndent; }
        void pushIndent() 
        { 
            for( int i = 0; i != INDENT; ++i )
                m_strIndent.push_back( ' ' );
        }
        void popIndent()
        {
            for( int i = 0; i != INDENT; ++i )
                m_strIndent.pop_back();
        }
        
        Printer::Ptr read( const concrete::Dimension* pDimension, const std::string& strIndex )
        {
            return m_printerFactory.read( m_layout.getDataMember( pDimension ), strIndex.c_str() );
        }
        
        Printer::Ptr write( const concrete::Dimension* pDimension, const std::string& strIndex )
        {
            return m_printerFactory.write( m_layout.getDataMember( pDimension ), strIndex.c_str() );
        }
        
        std::string getFailureReturnType() const { return m_strFailureType; }
        
        const Layout& getLayout() const { return m_layout; }
    private:
        VariableExprMap m_variables;
        std::string m_strIndent;
        const Layout& m_layout;
        std::string m_strFailureType;
        PrinterFactory& m_printerFactory;
    };
    
    class InstructionCodeGenerator
    {
    public:
        InstructionCodeGenerator( CodeGenerator& _generator, std::ostream& _os );
    
        virtual ~InstructionCodeGenerator();
        
        virtual void generate( const RootInstruction& ins                          );
        virtual void generate( const ParentDerivationInstruction& ins              );
        virtual void generate( const ChildDerivationInstruction& ins               );
        virtual void generate( const EnumDerivationInstruction& ins                );
        virtual void generate( const EnumerationInstruction& ins                   );
        virtual void generate( const DimensionReferenceReadInstruction& ins        );
        virtual void generate( const MonomorphicReferenceInstruction& ins          );
        virtual void generate( const PolymorphicReferenceBranchInstruction& ins    );
        virtual void generate( const PolymorphicCaseInstruction& ins               );
        virtual void generate( const CallOperation& ins                            );
        virtual void generate( const StartOperation& ins                           );
        virtual void generate( const StopOperation& ins                            );
        virtual void generate( const PauseOperation& ins                           );
        virtual void generate( const ResumeOperation& ins                          );
        virtual void generate( const DoneOperation& ins                            );
        virtual void generate( const WaitActionOperation& ins                      );
        virtual void generate( const WaitDimensionOperation& ins                   );
        virtual void generate( const GetActionOperation& ins                       );
        virtual void generate( const GetDimensionOperation& ins                    );
        virtual void generate( const ReadOperation& ins                            );
        virtual void generate( const WriteOperation& ins                           );
        virtual void generate( const WriteLinkOperation& ins                       );
        virtual void generate( const RangeOperation& ins                           );
        virtual void generate( const Instruction& ins                              );
        
    protected:
        CodeGenerator& generator;
        std::ostream& os;
    };

    class InstructionCodeGeneratorFactory
    {
    public:
        virtual ~InstructionCodeGeneratorFactory();
        virtual std::shared_ptr< InstructionCodeGenerator > create( CodeGenerator& generator, std::ostream& os ) = 0;
    };
    
    class InstructionCodeGeneratorFactoryDefault : public InstructionCodeGeneratorFactory
    {
    public:
        std::shared_ptr< InstructionCodeGenerator > create( CodeGenerator& generator, std::ostream& os );
    };
}

#endif //INSRUCTION_CODE_GENERATOR_17_JULY_2020