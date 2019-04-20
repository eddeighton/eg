
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
        
    private:
        const concrete::Dimension* m_pDimension;
        std::size_t offset;
        std::string name;
        Buffer* m_pBuffer;
        DataMember* m_pDependency = nullptr;
    };
    
    struct Printer
    {
        const DataMember* m_pDimension;
        const char* pszIndex;
        Printer( const DataMember* pDimension, const char* pszIndex ) : m_pDimension( pDimension ), pszIndex( pszIndex ) {}
    };
    inline std::ostream& operator<<( std::ostream& os, const Printer& printer )
    {
        printer.m_pDimension->printVariableAccess( os, printer.pszIndex );;
        return os;
    }
    
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
        using DimensionMap = std::map< const concrete::Dimension*, DataMember* >;
        using DimensionMapCst = std::map< const concrete::Dimension*, const DataMember* >;
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