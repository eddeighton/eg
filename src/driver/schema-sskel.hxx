// Copyright (c) 2005-2011 Code Synthesis Tools CC
//
// This program was generated by CodeSynthesis XSD/e, an XML Schema
// to C++ data binding compiler for embedded systems.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License version 2 as
// published by the Free Software Foundation.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
//
//

#ifndef W__WORKSPACE_EG_SRC_DRIVER_SCHEMA_SSKEL_HXX
#define W__WORKSPACE_EG_SRC_DRIVER_SCHEMA_SSKEL_HXX

#include <xsde/cxx/version.hxx>

#if (XSDE_INT_VERSION != 3020000L)
#error XSD/e runtime version mismatch
#endif

#include <xsde/cxx/config.hxx>

#ifndef XSDE_ENCODING_UTF8
#error the generated code uses the UTF-8 encodingwhile the XSD/e runtime does not (reconfigure the runtime or change the --char-encoding value)
#endif

#ifndef XSDE_STL
#error the generated code uses STL while the XSD/e runtime does not (reconfigure the runtime or add --no-stl)
#endif

#ifndef XSDE_IOSTREAM
#error the generated code uses iostream while the XSD/e runtime does not (reconfigure the runtime or add --no-iostream)
#endif

#ifndef XSDE_EXCEPTIONS
#error the generated code uses exceptions while the XSD/e runtime does not (reconfigure the runtime or add --no-exceptions)
#endif

#ifdef XSDE_LONGLONG
#error the XSD/e runtime uses long long while the generated code does not (reconfigure the runtime or remove --no-long-long)
#endif

#ifndef XSDE_SERIALIZER_VALIDATION
#error the generated code uses validation while the XSD/e runtime does not (reconfigure the runtime or add --suppress-validation)
#endif

#ifdef XSDE_POLYMORPHIC
#error the generated code expects XSD/e runtime without polymorphism support (reconfigure the runtime or add --generate-polymorphic/--runtime-polymorphic)
#endif

#ifndef XSDE_REUSE_STYLE_TIEIN
#error the generated code uses the tiein reuse style while the XSD/e runtime does not (reconfigure the runtime or add --reuse-style-mixin or --reuse-style-none)
#endif

#ifdef XSDE_CUSTOM_ALLOCATOR
#error the XSD/e runtime uses custom allocator while the generated code does not (reconfigure the runtime or add --custom-allocator)
#endif

#include <xsde/cxx/pre.hxx>

// Begin prologue.
//

#include <xsde/cxx/hybrid/any-type.hxx>
#include <xsde/cxx/hybrid/any-type-sskel.hxx>
#include <xsde/cxx/hybrid/any-type-simpl.hxx>

namespace xml_schema
{
  using ::xsde::cxx::hybrid::any_type;

  using ::xsde::cxx::hybrid::any_type_sskel;
  using ::xsde::cxx::hybrid::any_type_simpl;
}


//
// End prologue.

// Forward declarations
//
namespace egxml
{
  class Package_sskel;
  class Host_sskel;
  class Build_sskel;
  class Stack_sskel;
  class Fibers_sskel;
  class Defaults_sskel;
  class Project_sskel;
  class EG_sskel;
  class Directories_sskel;
  class Files_sskel;
  class Run_sskel;
}


#include <string>

#include <xsde/cxx/serializer/xml-schema.hxx>

#include <xsde/cxx/serializer/exceptions.hxx>

#include <xsde/cxx/serializer/validating/serializer.hxx>
#include <xsde/cxx/serializer/validating/xml-schema-sskel.hxx>
#include <xsde/cxx/serializer/validating/xml-schema-simpl.hxx>

#include <xsde/cxx/serializer/genx/document.hxx>

#include "schema.hxx"

namespace xml_schema
{
  // Built-in XML Schema types mapping.
  //
  using ::xsde::cxx::string_sequence;
  using ::xsde::cxx::qname;
  using ::xsde::cxx::buffer;
  using ::xsde::cxx::time_zone;
  using ::xsde::cxx::gday;
  using ::xsde::cxx::gmonth;
  using ::xsde::cxx::gyear;
  using ::xsde::cxx::gmonth_day;
  using ::xsde::cxx::gyear_month;
  using ::xsde::cxx::date;
  using ::xsde::cxx::time;
  using ::xsde::cxx::date_time;
  using ::xsde::cxx::duration;

  // Base serializer skeletons.
  //
  using ::xsde::cxx::serializer::serializer_base;
  typedef ::xsde::cxx::serializer::validating::empty_content serializer_empty_content;
  typedef ::xsde::cxx::serializer::validating::simple_content serializer_simple_content;
  typedef ::xsde::cxx::serializer::validating::complex_content serializer_complex_content;

  // Serializer skeletons and implementations for the
  // XML Schema built-in types.
  //
  using ::xsde::cxx::serializer::validating::any_simple_type_sskel;
  using ::xsde::cxx::serializer::validating::any_simple_type_simpl;

  using ::xsde::cxx::serializer::validating::byte_sskel;
  using ::xsde::cxx::serializer::validating::byte_simpl;

  using ::xsde::cxx::serializer::validating::unsigned_byte_sskel;
  using ::xsde::cxx::serializer::validating::unsigned_byte_simpl;

  using ::xsde::cxx::serializer::validating::short_sskel;
  using ::xsde::cxx::serializer::validating::short_simpl;

  using ::xsde::cxx::serializer::validating::unsigned_short_sskel;
  using ::xsde::cxx::serializer::validating::unsigned_short_simpl;

  using ::xsde::cxx::serializer::validating::int_sskel;
  using ::xsde::cxx::serializer::validating::int_simpl;

  using ::xsde::cxx::serializer::validating::unsigned_int_sskel;
  using ::xsde::cxx::serializer::validating::unsigned_int_simpl;

  using ::xsde::cxx::serializer::validating::long_sskel;
  using ::xsde::cxx::serializer::validating::long_simpl;

  using ::xsde::cxx::serializer::validating::unsigned_long_sskel;
  using ::xsde::cxx::serializer::validating::unsigned_long_simpl;

  using ::xsde::cxx::serializer::validating::integer_sskel;
  using ::xsde::cxx::serializer::validating::integer_simpl;

  using ::xsde::cxx::serializer::validating::non_positive_integer_sskel;
  using ::xsde::cxx::serializer::validating::non_positive_integer_simpl;

  using ::xsde::cxx::serializer::validating::non_negative_integer_sskel;
  using ::xsde::cxx::serializer::validating::non_negative_integer_simpl;

  using ::xsde::cxx::serializer::validating::positive_integer_sskel;
  using ::xsde::cxx::serializer::validating::positive_integer_simpl;

  using ::xsde::cxx::serializer::validating::negative_integer_sskel;
  using ::xsde::cxx::serializer::validating::negative_integer_simpl;

  using ::xsde::cxx::serializer::validating::boolean_sskel;
  using ::xsde::cxx::serializer::validating::boolean_simpl;

  using ::xsde::cxx::serializer::validating::float_sskel;
  using ::xsde::cxx::serializer::validating::float_simpl;

  using ::xsde::cxx::serializer::validating::double_sskel;
  using ::xsde::cxx::serializer::validating::double_simpl;

  using ::xsde::cxx::serializer::validating::decimal_sskel;
  using ::xsde::cxx::serializer::validating::decimal_simpl;

  using ::xsde::cxx::serializer::validating::string_sskel;
  using ::xsde::cxx::serializer::validating::string_simpl;

  using ::xsde::cxx::serializer::validating::normalized_string_sskel;
  using ::xsde::cxx::serializer::validating::normalized_string_simpl;

  using ::xsde::cxx::serializer::validating::token_sskel;
  using ::xsde::cxx::serializer::validating::token_simpl;

  using ::xsde::cxx::serializer::validating::name_sskel;
  using ::xsde::cxx::serializer::validating::name_simpl;

  using ::xsde::cxx::serializer::validating::nmtoken_sskel;
  using ::xsde::cxx::serializer::validating::nmtoken_simpl;

  using ::xsde::cxx::serializer::validating::nmtokens_sskel;
  using ::xsde::cxx::serializer::validating::nmtokens_simpl;

  using ::xsde::cxx::serializer::validating::ncname_sskel;
  using ::xsde::cxx::serializer::validating::ncname_simpl;

  using ::xsde::cxx::serializer::validating::language_sskel;
  using ::xsde::cxx::serializer::validating::language_simpl;

  using ::xsde::cxx::serializer::validating::id_sskel;
  using ::xsde::cxx::serializer::validating::id_simpl;

  using ::xsde::cxx::serializer::validating::idref_sskel;
  using ::xsde::cxx::serializer::validating::idref_simpl;

  using ::xsde::cxx::serializer::validating::idrefs_sskel;
  using ::xsde::cxx::serializer::validating::idrefs_simpl;

  using ::xsde::cxx::serializer::validating::uri_sskel;
  using ::xsde::cxx::serializer::validating::uri_simpl;

  using ::xsde::cxx::serializer::validating::qname_sskel;
  using ::xsde::cxx::serializer::validating::qname_simpl;

  using ::xsde::cxx::serializer::validating::base64_binary_sskel;
  using ::xsde::cxx::serializer::validating::base64_binary_simpl;

  using ::xsde::cxx::serializer::validating::hex_binary_sskel;
  using ::xsde::cxx::serializer::validating::hex_binary_simpl;

  using ::xsde::cxx::serializer::validating::date_sskel;
  using ::xsde::cxx::serializer::validating::date_simpl;

  using ::xsde::cxx::serializer::validating::date_time_sskel;
  using ::xsde::cxx::serializer::validating::date_time_simpl;

  using ::xsde::cxx::serializer::validating::duration_sskel;
  using ::xsde::cxx::serializer::validating::duration_simpl;

  using ::xsde::cxx::serializer::validating::gday_sskel;
  using ::xsde::cxx::serializer::validating::gday_simpl;

  using ::xsde::cxx::serializer::validating::gmonth_sskel;
  using ::xsde::cxx::serializer::validating::gmonth_simpl;

  using ::xsde::cxx::serializer::validating::gmonth_day_sskel;
  using ::xsde::cxx::serializer::validating::gmonth_day_simpl;

  using ::xsde::cxx::serializer::validating::gyear_sskel;
  using ::xsde::cxx::serializer::validating::gyear_simpl;

  using ::xsde::cxx::serializer::validating::gyear_month_sskel;
  using ::xsde::cxx::serializer::validating::gyear_month_simpl;

  using ::xsde::cxx::serializer::validating::time_sskel;
  using ::xsde::cxx::serializer::validating::time_simpl;

  // Error codes.
  //
  typedef xsde::cxx::serializer::genx::xml_error serializer_xml_error;
  typedef xsde::cxx::schema_error serializer_schema_error;

  // Exceptions.
  //
  typedef xsde::cxx::serializer::exception serializer_exception;
  typedef xsde::cxx::serializer::xml serializer_xml;
  typedef xsde::cxx::serializer::schema serializer_schema;

  // Document serializer.
  //
  using xsde::cxx::serializer::genx::writer;
  using xsde::cxx::serializer::genx::document_simpl;

  // Serializer context.
  //
  typedef xsde::cxx::serializer::context serializer_context;
}

namespace egxml
{
  class Package_sskel: public ::xsde::cxx::serializer::validating::complex_content
  {
    public:
    // Serializer callbacks. Override them in your implementation.
    //

    virtual void
    pre (const ::egxml::Package&) = 0;

    // Elements.
    //
    virtual ::std::string
    Name () = 0;

    virtual bool
    Repository_present ();

    virtual ::std::string
    Repository () = 0;

    virtual ::std::string
    License () = 0;

    virtual bool
    Description_present ();

    virtual ::std::string
    Description () = 0;

    virtual bool
    Directories_present ();

    virtual const ::egxml::Directories&
    Directories () = 0;

    virtual bool
    Files_present ();

    virtual const ::egxml::Files&
    Files () = 0;

    virtual bool
    Command_next ();

    virtual ::std::string
    Command () = 0;

    // virtual void
    // post ();

    // Serializer construction API.
    //
    void
    serializers (::xml_schema::string_sskel& /* Name */,
                 ::xml_schema::string_sskel& /* Repository */,
                 ::xml_schema::string_sskel& /* License */,
                 ::xml_schema::string_sskel& /* Description */,
                 ::egxml::Directories_sskel& /* Directories */,
                 ::egxml::Files_sskel& /* Files */,
                 ::xml_schema::string_sskel& /* Command */);

    // Individual element serializers.
    //
    void
    Name_serializer (::xml_schema::string_sskel&);

    void
    Repository_serializer (::xml_schema::string_sskel&);

    void
    License_serializer (::xml_schema::string_sskel&);

    void
    Description_serializer (::xml_schema::string_sskel&);

    void
    Directories_serializer (::egxml::Directories_sskel&);

    void
    Files_serializer (::egxml::Files_sskel&);

    void
    Command_serializer (::xml_schema::string_sskel&);

    virtual void
    _reset ();

    // Constructor.
    //
    Package_sskel ();

    // Implementation.
    //
    public:
    virtual void
    _serialize_content ();

    protected:
    Package_sskel* Package_impl_;
    Package_sskel (Package_sskel*, void*);

    protected:
    ::xml_schema::string_sskel* Name_serializer_;
    ::xml_schema::string_sskel* Repository_serializer_;
    ::xml_schema::string_sskel* License_serializer_;
    ::xml_schema::string_sskel* Description_serializer_;
    ::egxml::Directories_sskel* Directories_serializer_;
    ::egxml::Files_sskel* Files_serializer_;
    ::xml_schema::string_sskel* Command_serializer_;
  };

  class Host_sskel: public ::egxml::Package_sskel
  {
    public:
    // Serializer callbacks. Override them in your implementation.
    //

    virtual void
    pre (const ::egxml::Host&) = 0;

    // virtual void
    // post ();

    // Constructor.
    //
    Host_sskel (::egxml::Package_sskel* tiein);

    // Implementation.
    //
    public:
    virtual void
    pre (const ::egxml::Package&);

    virtual ::std::string
    Name ();

    virtual ::std::string
    Repository ();

    virtual ::std::string
    License ();

    virtual ::std::string
    Description ();

    virtual const ::egxml::Directories&
    Directories ();

    virtual const ::egxml::Files&
    Files ();

    virtual ::std::string
    Command ();

    protected:
    Host_sskel* Host_impl_;
    Host_sskel (Host_sskel*, void*);
  };

  class Build_sskel: public ::xsde::cxx::serializer::validating::complex_content
  {
    public:
    // Serializer callbacks. Override them in your implementation.
    //

    virtual void
    pre (const ::egxml::Build&) = 0;

    // Elements.
    //
    virtual ::std::string
    Name () = 0;

    virtual ::std::string
    CompilerFlags () = 0;

    virtual ::std::string
    LinkerFlags () = 0;

    // virtual void
    // post ();

    // Serializer construction API.
    //
    void
    serializers (::xml_schema::string_sskel& /* Name */,
                 ::xml_schema::string_sskel& /* CompilerFlags */,
                 ::xml_schema::string_sskel& /* LinkerFlags */);

    // Individual element serializers.
    //
    void
    Name_serializer (::xml_schema::string_sskel&);

    void
    CompilerFlags_serializer (::xml_schema::string_sskel&);

    void
    LinkerFlags_serializer (::xml_schema::string_sskel&);

    virtual void
    _reset ();

    // Constructor.
    //
    Build_sskel ();

    // Implementation.
    //
    public:
    virtual void
    _serialize_content ();

    protected:
    Build_sskel* Build_impl_;
    Build_sskel (Build_sskel*, void*);

    protected:
    ::xml_schema::string_sskel* Name_serializer_;
    ::xml_schema::string_sskel* CompilerFlags_serializer_;
    ::xml_schema::string_sskel* LinkerFlags_serializer_;
  };

  class Stack_sskel: public ::xsde::cxx::serializer::validating::complex_content
  {
    public:
    // Serializer callbacks. Override them in your implementation.
    //

    virtual void
    pre (const ::egxml::Stack&) = 0;

    // Elements.
    //
    virtual bool
    Size_present ();

    virtual unsigned int
    Size () = 0;

    // virtual void
    // post ();

    // Serializer construction API.
    //
    void
    serializers (::xml_schema::unsigned_int_sskel& /* Size */);

    // Individual element serializers.
    //
    void
    Size_serializer (::xml_schema::unsigned_int_sskel&);

    virtual void
    _reset ();

    // Constructor.
    //
    Stack_sskel ();

    // Implementation.
    //
    public:
    virtual void
    _serialize_content ();

    protected:
    Stack_sskel* Stack_impl_;
    Stack_sskel (Stack_sskel*, void*);

    protected:
    ::xml_schema::unsigned_int_sskel* Size_serializer_;
  };

  class Fibers_sskel: public ::xsde::cxx::serializer::validating::complex_content
  {
    public:
    // Serializer callbacks. Override them in your implementation.
    //

    virtual void
    pre (const ::egxml::Fibers&) = 0;

    // Elements.
    //
    virtual bool
    Stack_present ();

    virtual const ::egxml::Stack&
    Stack () = 0;

    // virtual void
    // post ();

    // Serializer construction API.
    //
    void
    serializers (::egxml::Stack_sskel& /* Stack */);

    // Individual element serializers.
    //
    void
    Stack_serializer (::egxml::Stack_sskel&);

    virtual void
    _reset ();

    // Constructor.
    //
    Fibers_sskel ();

    // Implementation.
    //
    public:
    virtual void
    _serialize_content ();

    protected:
    Fibers_sskel* Fibers_impl_;
    Fibers_sskel (Fibers_sskel*, void*);

    protected:
    ::egxml::Stack_sskel* Stack_serializer_;
  };

  class Defaults_sskel: public ::xsde::cxx::serializer::validating::complex_content
  {
    public:
    // Serializer callbacks. Override them in your implementation.
    //

    virtual void
    pre (const ::egxml::Defaults&) = 0;

    // Elements.
    //
    virtual bool
    Fibers_present ();

    virtual const ::egxml::Fibers&
    Fibers () = 0;

    // virtual void
    // post ();

    // Serializer construction API.
    //
    void
    serializers (::egxml::Fibers_sskel& /* Fibers */);

    // Individual element serializers.
    //
    void
    Fibers_serializer (::egxml::Fibers_sskel&);

    virtual void
    _reset ();

    // Constructor.
    //
    Defaults_sskel ();

    // Implementation.
    //
    public:
    virtual void
    _serialize_content ();

    protected:
    Defaults_sskel* Defaults_impl_;
    Defaults_sskel (Defaults_sskel*, void*);

    protected:
    ::egxml::Fibers_sskel* Fibers_serializer_;
  };

  class Project_sskel: public ::xsde::cxx::serializer::validating::complex_content
  {
    public:
    // Serializer callbacks. Override them in your implementation.
    //

    virtual void
    pre (const ::egxml::Project&) = 0;

    // Elements.
    //
    virtual ::std::string
    Name () = 0;

    virtual ::std::string
    Host () = 0;

    virtual bool
    Package_next ();

    virtual ::std::string
    Package () = 0;

    virtual bool
    Build_next () = 0;

    virtual const ::egxml::Build&
    Build () = 0;

    virtual bool
    Run_next () = 0;

    virtual const ::egxml::Run&
    Run () = 0;

    virtual bool
    Defaults_present ();

    virtual const ::egxml::Defaults&
    Defaults () = 0;

    // virtual void
    // post ();

    // Serializer construction API.
    //
    void
    serializers (::xml_schema::string_sskel& /* Name */,
                 ::xml_schema::string_sskel& /* Host */,
                 ::xml_schema::string_sskel& /* Package */,
                 ::egxml::Build_sskel& /* Build */,
                 ::egxml::Run_sskel& /* Run */,
                 ::egxml::Defaults_sskel& /* Defaults */);

    // Individual element serializers.
    //
    void
    Name_serializer (::xml_schema::string_sskel&);

    void
    Host_serializer (::xml_schema::string_sskel&);

    void
    Package_serializer (::xml_schema::string_sskel&);

    void
    Build_serializer (::egxml::Build_sskel&);

    void
    Run_serializer (::egxml::Run_sskel&);

    void
    Defaults_serializer (::egxml::Defaults_sskel&);

    virtual void
    _reset ();

    // Constructor.
    //
    Project_sskel ();

    // Implementation.
    //
    public:
    virtual void
    _serialize_content ();

    protected:
    Project_sskel* Project_impl_;
    Project_sskel (Project_sskel*, void*);

    protected:
    ::xml_schema::string_sskel* Name_serializer_;
    ::xml_schema::string_sskel* Host_serializer_;
    ::xml_schema::string_sskel* Package_serializer_;
    ::egxml::Build_sskel* Build_serializer_;
    ::egxml::Run_sskel* Run_serializer_;
    ::egxml::Defaults_sskel* Defaults_serializer_;
  };

  class EG_sskel: public ::xsde::cxx::serializer::validating::complex_content
  {
    public:
    // Serializer callbacks. Override them in your implementation.
    //

    virtual void
    pre (const ::egxml::EG&) = 0;

    // Elements.
    //
    enum choice_arm_tag
    {
      Package_tag,
      Host_tag,
      Project_tag
    };

    virtual choice_arm_tag
    choice_arm () = 0;

    virtual const ::egxml::Package&
    Package () = 0;

    virtual const ::egxml::Host&
    Host () = 0;

    virtual const ::egxml::Project&
    Project () = 0;

    // virtual void
    // post ();

    // Serializer construction API.
    //
    void
    serializers (::egxml::Package_sskel& /* Package */,
                 ::egxml::Host_sskel& /* Host */,
                 ::egxml::Project_sskel& /* Project */);

    // Individual element serializers.
    //
    void
    Package_serializer (::egxml::Package_sskel&);

    void
    Host_serializer (::egxml::Host_sskel&);

    void
    Project_serializer (::egxml::Project_sskel&);

    virtual void
    _reset ();

    // Constructor.
    //
    EG_sskel ();

    // Implementation.
    //
    public:
    virtual void
    _serialize_content ();

    protected:
    EG_sskel* EG_impl_;
    EG_sskel (EG_sskel*, void*);

    protected:
    ::egxml::Package_sskel* Package_serializer_;
    ::egxml::Host_sskel* Host_serializer_;
    ::egxml::Project_sskel* Project_serializer_;
  };

  class Directories_sskel: public ::xsde::cxx::serializer::validating::complex_content
  {
    public:
    // Serializer callbacks. Override them in your implementation.
    //

    virtual void
    pre (const ::egxml::Directories&) = 0;

    // Elements.
    //
    virtual bool
    Include_next ();

    virtual ::std::string
    Include () = 0;

    virtual bool
    Library_next ();

    virtual ::std::string
    Library () = 0;

    // virtual void
    // post ();

    // Serializer construction API.
    //
    void
    serializers (::xml_schema::string_sskel& /* Include */,
                 ::xml_schema::string_sskel& /* Library */);

    // Individual element serializers.
    //
    void
    Include_serializer (::xml_schema::string_sskel&);

    void
    Library_serializer (::xml_schema::string_sskel&);

    virtual void
    _reset ();

    // Constructor.
    //
    Directories_sskel ();

    // Implementation.
    //
    public:
    virtual void
    _serialize_content ();

    protected:
    Directories_sskel* Directories_impl_;
    Directories_sskel (Directories_sskel*, void*);

    protected:
    ::xml_schema::string_sskel* Include_serializer_;
    ::xml_schema::string_sskel* Library_serializer_;
  };

  class Files_sskel: public ::xsde::cxx::serializer::validating::complex_content
  {
    public:
    // Serializer callbacks. Override them in your implementation.
    //

    virtual void
    pre (const ::egxml::Files&) = 0;

    // Elements.
    //
    virtual bool
    System_next ();

    virtual ::std::string
    System () = 0;

    virtual bool
    Include_next ();

    virtual ::std::string
    Include () = 0;

    virtual bool
    Source_next ();

    virtual ::std::string
    Source () = 0;

    virtual bool
    Library_next ();

    virtual ::std::string
    Library () = 0;

    // virtual void
    // post ();

    // Serializer construction API.
    //
    void
    serializers (::xml_schema::string_sskel& /* System */,
                 ::xml_schema::string_sskel& /* Include */,
                 ::xml_schema::string_sskel& /* Source */,
                 ::xml_schema::string_sskel& /* Library */);

    // Individual element serializers.
    //
    void
    System_serializer (::xml_schema::string_sskel&);

    void
    Include_serializer (::xml_schema::string_sskel&);

    void
    Source_serializer (::xml_schema::string_sskel&);

    void
    Library_serializer (::xml_schema::string_sskel&);

    virtual void
    _reset ();

    // Constructor.
    //
    Files_sskel ();

    // Implementation.
    //
    public:
    virtual void
    _serialize_content ();

    protected:
    Files_sskel* Files_impl_;
    Files_sskel (Files_sskel*, void*);

    protected:
    ::xml_schema::string_sskel* System_serializer_;
    ::xml_schema::string_sskel* Include_serializer_;
    ::xml_schema::string_sskel* Source_serializer_;
    ::xml_schema::string_sskel* Library_serializer_;
  };

  class Run_sskel: public ::xsde::cxx::serializer::validating::complex_content
  {
    public:
    // Serializer callbacks. Override them in your implementation.
    //

    virtual void
    pre (const ::egxml::Run&) = 0;

    // Elements.
    //
    virtual ::std::string
    Name () = 0;

    virtual ::std::string
    Command () = 0;

    virtual bool
    Argument_next ();

    virtual ::std::string
    Argument () = 0;

    // virtual void
    // post ();

    // Serializer construction API.
    //
    void
    serializers (::xml_schema::string_sskel& /* Name */,
                 ::xml_schema::string_sskel& /* Command */,
                 ::xml_schema::string_sskel& /* Argument */);

    // Individual element serializers.
    //
    void
    Name_serializer (::xml_schema::string_sskel&);

    void
    Command_serializer (::xml_schema::string_sskel&);

    void
    Argument_serializer (::xml_schema::string_sskel&);

    virtual void
    _reset ();

    // Constructor.
    //
    Run_sskel ();

    // Implementation.
    //
    public:
    virtual void
    _serialize_content ();

    protected:
    Run_sskel* Run_impl_;
    Run_sskel (Run_sskel*, void*);

    protected:
    ::xml_schema::string_sskel* Name_serializer_;
    ::xml_schema::string_sskel* Command_serializer_;
    ::xml_schema::string_sskel* Argument_serializer_;
  };
}

// Begin epilogue.
//
//
// End epilogue.

#include <xsde/cxx/post.hxx>

#endif // W__WORKSPACE_EG_SRC_DRIVER_SCHEMA_SSKEL_HXX
