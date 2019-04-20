

#ifndef EVENT_05_SEPT_2018
#define EVENT_05_SEPT_2018

#include <string>

namespace IPC
{
    namespace Event
    {
        
        
        class Event
        {
            friend class File;
            static const std::size_t DELIM = 0xF0BAF0BA;
            struct Header
            {
                std::size_t reserved;
                std::size_t typeSize;
                std::size_t valueSize;
                std::size_t timestamp;
            }m_header;
            const char* m_pszType;
            const void* m_pValue;
        public:
            Event()
            {
                m_header.reserved       = DELIM;
                m_header.typeSize       = 0U;
                m_header.valueSize      = 0U;
                m_header.timestamp       = 0U;
                m_pszType               = nullptr;
                m_pValue                = nullptr;
            }
            
            Event( const char* pszType, std::size_t szTypeSize, std::size_t timestamp, 
                const void* pValue, std::size_t szValueSize )
            {
                m_header.reserved       = DELIM;
                m_header.typeSize       = szTypeSize + 1U;
                m_header.valueSize      = szValueSize;
                m_header.timestamp      = timestamp;
                m_pszType               = pszType;
                m_pValue                = pValue;
            }
            
            template< class T >
            Event( const char* pszType, std::size_t szTypeSize, std::size_t timestamp, const T& value )
            {
                m_header.reserved       = DELIM;
                m_header.typeSize       = szTypeSize + 1U;
                m_header.valueSize      = sizeof( T );
                m_header.timestamp      = timestamp;
                m_pszType               = pszType;
                m_pValue                = &value;
            }
            
            template< class T >
            Event( const std::string& strType, std::size_t timestamp, const T& value )
            {
                m_header.reserved       = DELIM;
                m_header.typeSize       = strType.size() + 1U;
                m_header.valueSize      = sizeof( T );
                m_header.timestamp      = timestamp;
                m_pszType               = strType.data();
                m_pValue                = &value;
            }
            
            inline const std::size_t getTimeStamp() const { return m_header.timestamp; }
            
            const std::string getType() const
            {
                return std::string( m_pszType, m_pszType + m_header.typeSize );
            }
            
            const char* getType_c_str() const
            {
                return m_pszType;
            }
            
            const size_t totalSize() const
            {
                return sizeof( Header ) + m_header.typeSize + m_header.valueSize;
            }
            
            const void* getValue()      const { return m_pValue; }
            std::size_t getValueSize()  const { return m_header.valueSize; }
            
        private:
            static inline bool isEvent( const char* pData )
            {
                const std::size_t szDelim = *reinterpret_cast< const std::size_t* >( pData );
                return szDelim == Event::DELIM;
            }
        
            inline void read( const char* pData )
            {
                m_header  = *reinterpret_cast< const Header* >( pData );
                m_pszType = pData + sizeof( Header );
                m_pValue  = pData + sizeof( Header ) + m_header.typeSize;
            }
            
            inline void write( char* pData ) const
            {
                memcpy( pData, (void*)&m_header, sizeof( Event::Header ) );
                memcpy( pData + sizeof( Header ), m_pszType, m_header.typeSize );
                memcpy( pData + sizeof( Header ) + m_header.typeSize, m_pValue, m_header.valueSize );
            }
        };
    }
}


#endif //EVENT_05_SEPT_2018