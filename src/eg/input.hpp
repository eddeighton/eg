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


#ifndef INPUT_TREE_18_04_2019
#define INPUT_TREE_18_04_2019

#include "objects.hpp"

#include "io/indexed_object.hpp"
#include "io/loader.hpp"
#include "io/storer.hpp"

#include <boost/filesystem/path.hpp>

#include <string>
#include <vector>
#include <optional>

namespace eg
{
    class Parser;
    class ObjectFactoryImpl;
    
    namespace input
    {
        class Element : public IndexedObject
        {
            friend class ::eg::ObjectFactoryImpl;
        protected:
            Element( const IndexedObject& object );
        public:
            virtual void print( std::ostream& os, std::string& strIndent, const std::string& strAnnotation ) const { };
        };

        class Opaque : public Element
        {
            friend class ObjectFactoryImpl;
            friend class ::eg::Parser;
        public:
            static const ObjectType Type = eInputOpaque;
            
            static bool equalNullablePtrs( const Opaque* pLeft, const Opaque* pRight ) 
            {
                if( pLeft && pRight )
                {
                    return pLeft->getStr() == pRight->getStr();
                }
                else 
                {
                    return ( pLeft == nullptr ) && ( pRight == nullptr );
                }
            }
        protected:
            Opaque( const IndexedObject& object );
        public:
            const std::string& getStr() const { return m_str; }
            bool isSemantic() const { return m_bSemantic; }
            virtual void load( Loader& loader );
            virtual void store( Storer& storer ) const;
            void print( std::ostream& os, std::string& strIndent, const std::string& strAnnotation ) const;
            
            void modify( const std::string& strNew ) { m_str = strNew; }
        private:
            std::string m_str;
            bool m_bSemantic;
        };

        class Dimension : public Element
        {
            friend class ::eg::ObjectFactoryImpl;
            friend class ::eg::Parser;
        public:
            static const ObjectType Type = eInputDimension;
        protected:
            Dimension( const IndexedObject& object );
        public:
            const std::string& getIdentifier() const { return m_strIdentifier; }
            const Opaque* getType() const { return m_pType; }
        
            virtual void load( Loader& loader );
            virtual void store( Storer& storer ) const;
            void print( std::ostream& os, std::string& strIndent, const std::string& strAnnotation ) const;
            
            bool equal( const Dimension& cmp ) const
            {
                return Opaque::equalNullablePtrs( m_pType, cmp.m_pType );
            }
                
        private:
            std::string m_strIdentifier;
            Opaque* m_pType;
        };

        class Include : public Element
        {
            friend class ::eg::ObjectFactoryImpl;
            friend class ::eg::Parser;
        public:
            static const ObjectType Type = eInputInclude;
        protected:
            Include( const IndexedObject& object );
        public:
            const std::string& getIdentifier() const { return m_strIdentifier; }
            const boost::filesystem::path& getIncludeFilePath() const { return m_path; }
            bool isEGInclude() const { return m_bIsEGInclude; }
            bool isSystemInclude() const { return m_bIsSystemInclude; }
            
            virtual void load( Loader& loader );
            virtual void store( Storer& storer ) const;
            void print( std::ostream& os, std::string& strIndent, const std::string& strAnnotation ) const;
            void setIncludeFilePath( const std::string& strIncludeFile );
            
            bool equal( const Include& cmp ) const
            {
                return m_path == cmp.m_path;
            }
        private:
            std::string m_strIdentifier;
            boost::filesystem::path m_path;
            bool m_bIsEGInclude;
            bool m_bIsSystemInclude;
        };

        class Using : public Element
        {
            friend class ::eg::ObjectFactoryImpl;
            friend class ::eg::Parser;
        public:
            static const ObjectType Type = eInputUsing;
        protected:
            Using( const IndexedObject& object );
        public:
            const std::string& getIdentifier() const { return m_strIdentifier; }
            const Opaque* getType() const { return m_pType; }
            
            virtual void load( Loader& loader );
            virtual void store( Storer& storer ) const;
            void print( std::ostream& os, std::string& strIndent, const std::string& strAnnotation ) const;
            
            bool equal( const Using& cmp ) const
            {
                return Opaque::equalNullablePtrs( m_pType, cmp.m_pType );
            }
        private:
            std::string m_strIdentifier;
            Opaque* m_pType;
        };

        class Action : public Element
        {
            friend class ::eg::ObjectFactoryImpl;
            friend class ::eg::Parser;
        public:
            static const ObjectType Type = eInputAction;
        protected:
            Action( const IndexedObject& object );
        public:
            const std::vector< Element* >& getElements() const { return m_elements; }
            const Opaque* getSize() const { return m_pSize; }
            const Opaque* getParams() const { return m_pParams; }
            const Opaque* getBody() const { return m_pBody; }
            const std::string& getIdentifier() const { return m_strIdentifier; }
            const std::vector< Opaque* >& getInheritance() const { return m_inheritance; }
            bool isLink() const { return m_bLink; }
            bool isAbstract() const { return m_bAbstract; }
            std::optional< boost::filesystem::path > getDefinitionFile() const { return m_definitionFile; }
            
            virtual void load( Loader& loader );
            virtual void store( Storer& storer ) const;
            Action* findAction( const std::string& strIdentifier ) const;
            void printDeclaration( std::ostream& os, std::string& strIndent, const std::string& strIdentifier, const std::string& strAnnotation  ) const;
            void print( std::ostream& os, std::string& strIndent, const std::string& strAnnotation ) const;

            bool equal( const Action& cmp ) const
            {
                return  Opaque::equalNullablePtrs( m_pSize, cmp.m_pSize ) &&
                        Opaque::equalNullablePtrs( m_pParams, cmp.m_pParams ) && 
                        m_definitionFile == cmp.m_definitionFile &&
                        m_bIsTemplate == cmp.m_bIsTemplate &&
                        m_bAbstract == cmp.m_bAbstract &&
                        m_bLink == cmp.m_bLink &&
                        std::equal( m_inheritance.begin(), m_inheritance.end(),
                            cmp.m_inheritance.begin(), cmp.m_inheritance.end(),
                            []( const Opaque* pLeft, const Opaque* pRight ) { return pLeft->getStr() == pRight->getStr(); } );
            }
        protected:
            std::vector< Element* > m_elements;
            Opaque* m_pSize;
            Opaque* m_pParams;
            Opaque* m_pBody;
            std::optional< boost::filesystem::path > m_definitionFile;
            bool m_bIsTemplate = false;
            std::string m_strIdentifier;
            bool m_bAbstract = false;
            bool m_bLink = false;
            std::vector< Opaque* > m_inheritance;
        };

        class Root : public Action
        {
            friend class ::eg::ObjectFactoryImpl;
            friend class ::eg::Parser;
        public:
            static const ObjectType Type = eInputRoot;
            static const std::string RootTypeName;
        protected:
            Root( const IndexedObject& object );
        public:
            std::optional< boost::filesystem::path > getIncludePath() const { return m_includePath; }
            
            virtual void load( Loader& loader );
            virtual void store( Storer& storer ) const;
            void print( std::ostream& os, std::string& strIndent, const std::string& strAnnotation ) const;
            
            bool equal( const Root& cmp ) const
            {
                return Action::equal( cmp ) && 
                    m_includePath == cmp.m_includePath;
            }
        private:
            std::optional< boost::filesystem::path > m_includePath; //null if main root
        };

    } //namespace input
} //namespace eg

#endif //INPUT_TREE_18_04_2019