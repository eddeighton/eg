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
            virtual void print( std::ostream& os, std::string& strIndent, IndexedObject::Index szIndex ) const { };
        };

        class Opaque : public Element
        {
            friend class ObjectFactoryImpl;
            friend class ::eg::Parser;
        public:
            static const ObjectType Type = eInputOpaque;
        protected:
            Opaque( const IndexedObject& object );
        public:
            const std::string& getStr() const { return m_str; }
            virtual void load( Loader& loader );
            virtual void store( Storer& storer ) const;
            void print( std::ostream& os, std::string& strIndent, IndexedObject::Index szIndex ) const;
        private:
            std::string m_str;
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
            void print( std::ostream& os, std::string& strIndent, IndexedObject::Index szIndex ) const;
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
            void print( std::ostream& os, std::string& strIndent, IndexedObject::Index szIndex ) const;
            void setIncludeFilePath( const std::string& strIncludeFile );
            
        private:
            std::string m_strIdentifier;
            boost::filesystem::path m_path;
            bool m_bIsEGInclude;
            bool m_bIsSystemInclude;
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
            const std::string& getIdentifier() const { return m_strIdentifier; }
            const std::vector< Opaque* >& getInheritance() const { return m_inheritance; }
            bool isLink() const { return m_bLink; }
            bool isAbstract() const { return m_bAbstract; }
            
            virtual void load( Loader& loader );
            virtual void store( Storer& storer ) const;
            Action* findAction( const std::string& strIdentifier ) const;
            void printDeclaration( std::ostream& os, std::string& strIndent, IndexedObject::Index szIndex ) const;
            void print( std::ostream& os, std::string& strIndent, IndexedObject::Index szIndex ) const;

        protected:
            std::vector< Element* > m_elements;
            Opaque* m_pSize;
            bool m_bDefined = false;
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
            const boost::filesystem::path& getPath() const { return m_path; }
            bool isMainFile() const { return m_bMainFile; }
            
            virtual void load( Loader& loader );
            virtual void store( Storer& storer ) const;
            void print( std::ostream& os, std::string& strIndent, IndexedObject::Index szIndex ) const;
        private:
            boost::filesystem::path m_path;
            bool m_bMainFile;
        };

    } //namespace input
} //namespace eg

#endif //INPUT_TREE_18_04_2019