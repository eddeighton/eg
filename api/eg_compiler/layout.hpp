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


#ifndef LAYOUT_18_02_2019
#define LAYOUT_18_02_2019

#include "concrete.hpp"

namespace eg
{
    class AnalysisSession;
    class Buffer;
    class Layout;
    class DataMember : public IndexedObject, public ::eg::concrete::IPrintDimensions
    {
        friend class ObjectFactoryImpl;
        friend class ImplementationSession;
    public:
        static const ObjectType Type = eDataMember;
    protected:
        DataMember( const IndexedObject& object )
            :  IndexedObject( object )
        {
        }
    public:
        virtual void load( Loader& loader );
        virtual void store( Storer& storer ) const;
        
        
        void print( std::ostream& os ) const;
        
        const concrete::Dimension* getInstanceDimension() const { return m_pDimension; }
        
        const std::string& getName() const { return name; }
        std::size_t getOffset() const { return offset; }
        const Buffer* getBuffer() const { return m_pBuffer; }
        
        virtual void printVariableAccess( std::ostream& os, const std::string& strIndex ) const;
        virtual void printDependencyVar( std::ostream& os, const std::string& strIndex ) const;
        void printAllocation( std::ostream& os, const std::string& strIndex ) const;
        void printDeallocation( std::ostream& os, const std::string& strIndex ) const;
        void printStart( std::ostream& os, const std::string& strIndex ) const;
        void printStop( std::ostream& os, const std::string& strIndex ) const;
        void printEncode( std::ostream& os, const std::string& strIndex ) const;
        void printDecode( std::ostream& os, const std::string& strIndex ) const;
    private:
        const concrete::Dimension* m_pDimension;
        std::size_t offset;
        std::string name;
        Buffer* m_pBuffer;
        DataMember* m_pDependency = nullptr;
    };
    
    class Buffer : public IndexedObject
    {
        friend class ObjectFactoryImpl;
        friend class ImplementationSession;
    public:
        static const ObjectType Type = eBuffer;
    protected:
        Buffer( const IndexedObject& object )
            :  IndexedObject( object )
        {
        }
    public:
        virtual void load( Loader& loader );
        virtual void store( Storer& storer ) const;

        const std::string& getTypeName() const { return name; }
        const std::string& getVariableName() const { return variable; }
        std::size_t getSize() const { return size; }
        std::size_t getStride() const { return stride; }
        const std::vector< const DataMember* >& getDimensions() const { return m_dimensions; }
        const concrete::Action* getAction() const { return m_pAction; }
    private:
        const concrete::Action* m_pAction = nullptr;
        std::vector< const DataMember* > m_dimensions;
        std::size_t size, stride;
        std::string name, variable;
    };
    
    class Layout : public IndexedObject
    {
        friend class ObjectFactoryImpl;
        friend class ImplementationSession;
    public:
        static const ObjectType Type = eLayout;
    protected:
        Layout( const IndexedObject& object )
            :  IndexedObject( object )
        {
        }
    private:
        using DimensionMap = std::map< const concrete::Dimension*, DataMember*, CompareIndexedObjects >;
        using DimensionMapCst = std::map< const concrete::Dimension*, const DataMember*, CompareIndexedObjects >;
    public:
        virtual void load( Loader& loader );
        virtual void store( Storer& storer ) const;
        
        const std::vector< Buffer* > getBuffers() const { return m_buffers; }
        
        inline const DataMember* getDataMember( const concrete::Dimension* pDimension ) const
        {
            DimensionMapCst::const_iterator iFind = m_dimensionMap.find( pDimension );
            VERIFY_RTE( iFind != m_dimensionMap.end() );
            return iFind->second;
        }
    private:
        std::vector< Buffer* > m_buffers;
        DimensionMapCst m_dimensionMap;
        
    };

} //namespace eg

#endif //LAYOUT_18_02_2019