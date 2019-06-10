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

#ifndef W__WORKSPACE_EG_SRC_DRIVER_SCHEMA_PSKEL_HXX
#define W__WORKSPACE_EG_SRC_DRIVER_SCHEMA_PSKEL_HXX

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

#ifndef XSDE_PARSER_VALIDATION
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
#include <xsde/cxx/hybrid/any-type-pskel.hxx>
#include <xsde/cxx/hybrid/any-type-pimpl.hxx>

namespace xml_schema
{
  using ::xsde::cxx::hybrid::any_type;

  using ::xsde::cxx::hybrid::any_type_pskel;
  using ::xsde::cxx::hybrid::any_type_pimpl;
}


//
// End prologue.

// Forward declarations
//
namespace egxml
{
  class Package_pskel;
  class Host_pskel;
  class Build_pskel;
  class Project_pskel;
  class EG_pskel;
  class Directories_pskel;
  class Files_pskel;
  class Run_pskel;
}


#include <xsde/cxx/ro-string.hxx>

#include <xsde/cxx/parser/xml-schema.hxx>

#include <xsde/cxx/parser/exceptions.hxx>

#include <xsde/cxx/stack.hxx>
#include <xsde/cxx/parser/validating/parser.hxx>
#include <xsde/cxx/parser/validating/xml-schema-pskel.hxx>
#include <xsde/cxx/parser/validating/xml-schema-pimpl.hxx>

#include <xsde/cxx/parser/expat/document.hxx>

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

  // Base parser skeletons.
  //
  using ::xsde::cxx::parser::parser_base;
  typedef ::xsde::cxx::parser::validating::empty_content parser_empty_content;
  typedef ::xsde::cxx::parser::validating::simple_content parser_simple_content;
  typedef ::xsde::cxx::parser::validating::complex_content parser_complex_content;
  typedef ::xsde::cxx::parser::validating::list_base parser_list_base;

  // Parser skeletons and implementations for the XML Schema
  // built-in types.
  //
  using ::xsde::cxx::parser::validating::any_simple_type_pskel;
  using ::xsde::cxx::parser::validating::any_simple_type_pimpl;

  using ::xsde::cxx::parser::validating::byte_pskel;
  using ::xsde::cxx::parser::validating::byte_pimpl;

  using ::xsde::cxx::parser::validating::unsigned_byte_pskel;
  using ::xsde::cxx::parser::validating::unsigned_byte_pimpl;

  using ::xsde::cxx::parser::validating::short_pskel;
  using ::xsde::cxx::parser::validating::short_pimpl;

  using ::xsde::cxx::parser::validating::unsigned_short_pskel;
  using ::xsde::cxx::parser::validating::unsigned_short_pimpl;

  using ::xsde::cxx::parser::validating::int_pskel;
  using ::xsde::cxx::parser::validating::int_pimpl;

  using ::xsde::cxx::parser::validating::unsigned_int_pskel;
  using ::xsde::cxx::parser::validating::unsigned_int_pimpl;

  using ::xsde::cxx::parser::validating::long_pskel;
  using ::xsde::cxx::parser::validating::long_pimpl;

  using ::xsde::cxx::parser::validating::unsigned_long_pskel;
  using ::xsde::cxx::parser::validating::unsigned_long_pimpl;

  using ::xsde::cxx::parser::validating::integer_pskel;
  using ::xsde::cxx::parser::validating::integer_pimpl;

  using ::xsde::cxx::parser::validating::non_positive_integer_pskel;
  using ::xsde::cxx::parser::validating::non_positive_integer_pimpl;

  using ::xsde::cxx::parser::validating::non_negative_integer_pskel;
  using ::xsde::cxx::parser::validating::non_negative_integer_pimpl;

  using ::xsde::cxx::parser::validating::positive_integer_pskel;
  using ::xsde::cxx::parser::validating::positive_integer_pimpl;

  using ::xsde::cxx::parser::validating::negative_integer_pskel;
  using ::xsde::cxx::parser::validating::negative_integer_pimpl;

  using ::xsde::cxx::parser::validating::boolean_pskel;
  using ::xsde::cxx::parser::validating::boolean_pimpl;

  using ::xsde::cxx::parser::validating::float_pskel;
  using ::xsde::cxx::parser::validating::float_pimpl;

  using ::xsde::cxx::parser::validating::double_pskel;
  using ::xsde::cxx::parser::validating::double_pimpl;

  using ::xsde::cxx::parser::validating::decimal_pskel;
  using ::xsde::cxx::parser::validating::decimal_pimpl;

  using ::xsde::cxx::parser::validating::string_pskel;
  using ::xsde::cxx::parser::validating::string_pimpl;

  using ::xsde::cxx::parser::validating::normalized_string_pskel;
  using ::xsde::cxx::parser::validating::normalized_string_pimpl;

  using ::xsde::cxx::parser::validating::token_pskel;
  using ::xsde::cxx::parser::validating::token_pimpl;

  using ::xsde::cxx::parser::validating::name_pskel;
  using ::xsde::cxx::parser::validating::name_pimpl;

  using ::xsde::cxx::parser::validating::nmtoken_pskel;
  using ::xsde::cxx::parser::validating::nmtoken_pimpl;

  using ::xsde::cxx::parser::validating::nmtokens_pskel;
  using ::xsde::cxx::parser::validating::nmtokens_pimpl;

  using ::xsde::cxx::parser::validating::ncname_pskel;
  using ::xsde::cxx::parser::validating::ncname_pimpl;

  using ::xsde::cxx::parser::validating::language_pskel;
  using ::xsde::cxx::parser::validating::language_pimpl;

  using ::xsde::cxx::parser::validating::id_pskel;
  using ::xsde::cxx::parser::validating::id_pimpl;

  using ::xsde::cxx::parser::validating::idref_pskel;
  using ::xsde::cxx::parser::validating::idref_pimpl;

  using ::xsde::cxx::parser::validating::idrefs_pskel;
  using ::xsde::cxx::parser::validating::idrefs_pimpl;

  using ::xsde::cxx::parser::validating::uri_pskel;
  using ::xsde::cxx::parser::validating::uri_pimpl;

  using ::xsde::cxx::parser::validating::qname_pskel;
  using ::xsde::cxx::parser::validating::qname_pimpl;

  using ::xsde::cxx::parser::validating::base64_binary_pskel;
  using ::xsde::cxx::parser::validating::base64_binary_pimpl;

  using ::xsde::cxx::parser::validating::hex_binary_pskel;
  using ::xsde::cxx::parser::validating::hex_binary_pimpl;

  using ::xsde::cxx::parser::validating::date_pskel;
  using ::xsde::cxx::parser::validating::date_pimpl;

  using ::xsde::cxx::parser::validating::date_time_pskel;
  using ::xsde::cxx::parser::validating::date_time_pimpl;

  using ::xsde::cxx::parser::validating::duration_pskel;
  using ::xsde::cxx::parser::validating::duration_pimpl;

  using ::xsde::cxx::parser::validating::gday_pskel;
  using ::xsde::cxx::parser::validating::gday_pimpl;

  using ::xsde::cxx::parser::validating::gmonth_pskel;
  using ::xsde::cxx::parser::validating::gmonth_pimpl;

  using ::xsde::cxx::parser::validating::gmonth_day_pskel;
  using ::xsde::cxx::parser::validating::gmonth_day_pimpl;

  using ::xsde::cxx::parser::validating::gyear_pskel;
  using ::xsde::cxx::parser::validating::gyear_pimpl;

  using ::xsde::cxx::parser::validating::gyear_month_pskel;
  using ::xsde::cxx::parser::validating::gyear_month_pimpl;

  using ::xsde::cxx::parser::validating::time_pskel;
  using ::xsde::cxx::parser::validating::time_pimpl;

  // Read-only string.
  //
  using ::xsde::cxx::ro_string;

  // Error codes.
  //
  typedef xsde::cxx::parser::expat::xml_error parser_xml_error;
  typedef xsde::cxx::schema_error parser_schema_error;

  // Exceptions.
  //
  typedef xsde::cxx::parser::exception parser_exception;
  typedef xsde::cxx::parser::xml parser_xml;
  typedef xsde::cxx::parser::schema parser_schema;

  // Document parser.
  //
  using xsde::cxx::parser::expat::document_pimpl;

  // Parser context.
  //
  typedef xsde::cxx::parser::context parser_context;
}

namespace egxml
{
  class Package_pskel: public ::xsde::cxx::parser::validating::complex_content
  {
    public:
    // Parser callbacks. Override them in your implementation.
    //
    // virtual void
    // pre ();

    // Elements.
    //
    virtual void
    Name (const ::std::string&);

    virtual void
    Repository (const ::std::string&);

    virtual void
    License (const ::std::string&);

    virtual void
    Description (const ::std::string&);

    virtual void
    Directories (::egxml::Directories*);

    virtual void
    Files (::egxml::Files*);

    virtual ::egxml::Package*
    post_Package () = 0;

    // Parser construction API.
    //
    void
    parsers (::xml_schema::string_pskel& /* Name */,
             ::xml_schema::string_pskel& /* Repository */,
             ::xml_schema::string_pskel& /* License */,
             ::xml_schema::string_pskel& /* Description */,
             ::egxml::Directories_pskel& /* Directories */,
             ::egxml::Files_pskel& /* Files */);

    // Individual element parsers.
    //
    void
    Name_parser (::xml_schema::string_pskel&);

    void
    Repository_parser (::xml_schema::string_pskel&);

    void
    License_parser (::xml_schema::string_pskel&);

    void
    Description_parser (::xml_schema::string_pskel&);

    void
    Directories_parser (::egxml::Directories_pskel&);

    void
    Files_parser (::egxml::Files_pskel&);

    virtual void
    _reset ();

    // Constructor.
    //
    Package_pskel ();

    // Implementation details.
    //
    protected:
    Package_pskel* Package_impl_;
    Package_pskel (Package_pskel*, void*);

    protected:
    virtual bool
    _start_element_impl (const ::xsde::cxx::ro_string&,
                         const ::xsde::cxx::ro_string&);

    virtual bool
    _end_element_impl (const ::xsde::cxx::ro_string&,
                       const ::xsde::cxx::ro_string&);

    protected:
    ::xml_schema::string_pskel* Name_parser_;
    ::xml_schema::string_pskel* Repository_parser_;
    ::xml_schema::string_pskel* License_parser_;
    ::xml_schema::string_pskel* Description_parser_;
    ::egxml::Directories_pskel* Directories_parser_;
    ::egxml::Files_pskel* Files_parser_;

    public:
    struct v_state_descr_
    {
      void (::egxml::Package_pskel::*func) (
        unsigned long&,
        unsigned long&,
        const ::xsde::cxx::ro_string&,
        const ::xsde::cxx::ro_string&,
        bool);
      unsigned long state;
      unsigned long count;
    };

    struct v_state_
    {
      v_state_descr_ data[2UL];
      unsigned long size;
    };

    protected:
    v_state_ v_state_first_;
    ::xsde::cxx::stack v_state_stack_;

    virtual void
    _pre_e_validate ();

    virtual void
    _post_e_validate ();

    void
    sequence_0 (unsigned long&,
                unsigned long&,
                const ::xsde::cxx::ro_string&,
                const ::xsde::cxx::ro_string&,
                bool);
  };

  class Host_pskel: public ::egxml::Package_pskel
  {
    public:
    // Parser callbacks. Override them in your implementation.
    //
    // virtual void
    // pre ();

    // Elements.
    //
    virtual void
    Command (const ::std::string&);

    virtual ::egxml::Host*
    post_Host () = 0;

    // Parser construction API.
    //
    void
    parsers (::xml_schema::string_pskel& /* Name */,
             ::xml_schema::string_pskel& /* Repository */,
             ::xml_schema::string_pskel& /* License */,
             ::xml_schema::string_pskel& /* Description */,
             ::egxml::Directories_pskel& /* Directories */,
             ::egxml::Files_pskel& /* Files */,
             ::xml_schema::string_pskel& /* Command */);

    // Individual element parsers.
    //
    void
    Command_parser (::xml_schema::string_pskel&);

    virtual void
    _reset ();

    // Constructor.
    //
    Host_pskel (::egxml::Package_pskel* tiein);

    // Implementation details.
    //
    virtual ::egxml::Package*
    post_Package ();

    protected:
    Host_pskel* Host_impl_;
    Host_pskel (Host_pskel*, void*);

    protected:
    virtual bool
    _start_element_impl (const ::xsde::cxx::ro_string&,
                         const ::xsde::cxx::ro_string&);

    virtual bool
    _end_element_impl (const ::xsde::cxx::ro_string&,
                       const ::xsde::cxx::ro_string&);

    protected:
    ::xml_schema::string_pskel* Command_parser_;

    public:
    struct v_state_descr_
    {
      void (::egxml::Host_pskel::*func) (
        unsigned long&,
        unsigned long&,
        const ::xsde::cxx::ro_string&,
        const ::xsde::cxx::ro_string&,
        bool);
      unsigned long state;
      unsigned long count;
    };

    struct v_state_
    {
      v_state_descr_ data[2UL];
      unsigned long size;
    };

    protected:
    v_state_ v_state_first_;
    ::xsde::cxx::stack v_state_stack_;

    virtual void
    _pre_e_validate ();

    virtual void
    _post_e_validate ();

    void
    sequence_0 (unsigned long&,
                unsigned long&,
                const ::xsde::cxx::ro_string&,
                const ::xsde::cxx::ro_string&,
                bool);
  };

  class Build_pskel: public ::xsde::cxx::parser::validating::complex_content
  {
    public:
    // Parser callbacks. Override them in your implementation.
    //
    // virtual void
    // pre ();

    // Elements.
    //
    virtual void
    Name (const ::std::string&);

    virtual void
    CompilerFlags (const ::std::string&);

    virtual void
    LinkerFlags (const ::std::string&);

    virtual ::egxml::Build
    post_Build () = 0;

    // Parser construction API.
    //
    void
    parsers (::xml_schema::string_pskel& /* Name */,
             ::xml_schema::string_pskel& /* CompilerFlags */,
             ::xml_schema::string_pskel& /* LinkerFlags */);

    // Individual element parsers.
    //
    void
    Name_parser (::xml_schema::string_pskel&);

    void
    CompilerFlags_parser (::xml_schema::string_pskel&);

    void
    LinkerFlags_parser (::xml_schema::string_pskel&);

    virtual void
    _reset ();

    // Constructor.
    //
    Build_pskel ();

    // Implementation details.
    //
    protected:
    Build_pskel* Build_impl_;
    Build_pskel (Build_pskel*, void*);

    protected:
    virtual bool
    _start_element_impl (const ::xsde::cxx::ro_string&,
                         const ::xsde::cxx::ro_string&);

    virtual bool
    _end_element_impl (const ::xsde::cxx::ro_string&,
                       const ::xsde::cxx::ro_string&);

    protected:
    ::xml_schema::string_pskel* Name_parser_;
    ::xml_schema::string_pskel* CompilerFlags_parser_;
    ::xml_schema::string_pskel* LinkerFlags_parser_;

    public:
    struct v_state_descr_
    {
      void (::egxml::Build_pskel::*func) (
        unsigned long&,
        unsigned long&,
        const ::xsde::cxx::ro_string&,
        const ::xsde::cxx::ro_string&,
        bool);
      unsigned long state;
      unsigned long count;
    };

    struct v_state_
    {
      v_state_descr_ data[2UL];
      unsigned long size;
    };

    protected:
    v_state_ v_state_first_;
    ::xsde::cxx::stack v_state_stack_;

    virtual void
    _pre_e_validate ();

    virtual void
    _post_e_validate ();

    void
    sequence_0 (unsigned long&,
                unsigned long&,
                const ::xsde::cxx::ro_string&,
                const ::xsde::cxx::ro_string&,
                bool);
  };

  class Project_pskel: public ::xsde::cxx::parser::validating::complex_content
  {
    public:
    // Parser callbacks. Override them in your implementation.
    //
    // virtual void
    // pre ();

    // Elements.
    //
    virtual void
    Name (const ::std::string&);

    virtual void
    Host (::egxml::Host*);

    virtual void
    Package (::egxml::Package*);

    virtual void
    Build (const ::egxml::Build&);

    virtual void
    Run (::egxml::Run*);

    virtual ::egxml::Project*
    post_Project () = 0;

    // Parser construction API.
    //
    void
    parsers (::xml_schema::string_pskel& /* Name */,
             ::egxml::Host_pskel& /* Host */,
             ::egxml::Package_pskel& /* Package */,
             ::egxml::Build_pskel& /* Build */,
             ::egxml::Run_pskel& /* Run */);

    // Individual element parsers.
    //
    void
    Name_parser (::xml_schema::string_pskel&);

    void
    Host_parser (::egxml::Host_pskel&);

    void
    Package_parser (::egxml::Package_pskel&);

    void
    Build_parser (::egxml::Build_pskel&);

    void
    Run_parser (::egxml::Run_pskel&);

    virtual void
    _reset ();

    // Constructor.
    //
    Project_pskel ();

    // Implementation details.
    //
    protected:
    Project_pskel* Project_impl_;
    Project_pskel (Project_pskel*, void*);

    protected:
    virtual bool
    _start_element_impl (const ::xsde::cxx::ro_string&,
                         const ::xsde::cxx::ro_string&);

    virtual bool
    _end_element_impl (const ::xsde::cxx::ro_string&,
                       const ::xsde::cxx::ro_string&);

    protected:
    ::xml_schema::string_pskel* Name_parser_;
    ::egxml::Host_pskel* Host_parser_;
    ::egxml::Package_pskel* Package_parser_;
    ::egxml::Build_pskel* Build_parser_;
    ::egxml::Run_pskel* Run_parser_;

    public:
    struct v_state_descr_
    {
      void (::egxml::Project_pskel::*func) (
        unsigned long&,
        unsigned long&,
        const ::xsde::cxx::ro_string&,
        const ::xsde::cxx::ro_string&,
        bool);
      unsigned long state;
      unsigned long count;
    };

    struct v_state_
    {
      v_state_descr_ data[2UL];
      unsigned long size;
    };

    protected:
    v_state_ v_state_first_;
    ::xsde::cxx::stack v_state_stack_;

    virtual void
    _pre_e_validate ();

    virtual void
    _post_e_validate ();

    void
    sequence_0 (unsigned long&,
                unsigned long&,
                const ::xsde::cxx::ro_string&,
                const ::xsde::cxx::ro_string&,
                bool);
  };

  class EG_pskel: public ::xsde::cxx::parser::validating::complex_content
  {
    public:
    // Parser callbacks. Override them in your implementation.
    //
    // virtual void
    // pre ();

    // Elements.
    //
    enum choice_arm_tag
    {
      Package_tag,
      Host_tag,
      Project_tag
    };

    virtual void
    choice_arm (choice_arm_tag);

    virtual void
    Package (::egxml::Package*);

    virtual void
    Host (::egxml::Host*);

    virtual void
    Project (::egxml::Project*);

    virtual ::egxml::EG*
    post_EG () = 0;

    // Parser construction API.
    //
    void
    parsers (::egxml::Package_pskel& /* Package */,
             ::egxml::Host_pskel& /* Host */,
             ::egxml::Project_pskel& /* Project */);

    // Individual element parsers.
    //
    void
    Package_parser (::egxml::Package_pskel&);

    void
    Host_parser (::egxml::Host_pskel&);

    void
    Project_parser (::egxml::Project_pskel&);

    virtual void
    _reset ();

    // Constructor.
    //
    EG_pskel ();

    // Implementation details.
    //
    protected:
    EG_pskel* EG_impl_;
    EG_pskel (EG_pskel*, void*);

    protected:
    virtual bool
    _start_element_impl (const ::xsde::cxx::ro_string&,
                         const ::xsde::cxx::ro_string&);

    virtual bool
    _end_element_impl (const ::xsde::cxx::ro_string&,
                       const ::xsde::cxx::ro_string&);

    protected:
    ::egxml::Package_pskel* Package_parser_;
    ::egxml::Host_pskel* Host_parser_;
    ::egxml::Project_pskel* Project_parser_;

    public:
    struct v_state_descr_
    {
      void (::egxml::EG_pskel::*func) (
        unsigned long&,
        unsigned long&,
        const ::xsde::cxx::ro_string&,
        const ::xsde::cxx::ro_string&,
        bool);
      unsigned long state;
      unsigned long count;
    };

    struct v_state_
    {
      v_state_descr_ data[2UL];
      unsigned long size;
    };

    protected:
    v_state_ v_state_first_;
    ::xsde::cxx::stack v_state_stack_;

    virtual void
    _pre_e_validate ();

    virtual void
    _post_e_validate ();

    void
    choice_0 (unsigned long&,
              unsigned long&,
              const ::xsde::cxx::ro_string&,
              const ::xsde::cxx::ro_string&,
              bool);
  };

  class Directories_pskel: public ::xsde::cxx::parser::validating::complex_content
  {
    public:
    // Parser callbacks. Override them in your implementation.
    //
    // virtual void
    // pre ();

    // Elements.
    //
    virtual void
    Include (const ::std::string&);

    virtual void
    Library (const ::std::string&);

    virtual ::egxml::Directories*
    post_Directories () = 0;

    // Parser construction API.
    //
    void
    parsers (::xml_schema::string_pskel& /* Include */,
             ::xml_schema::string_pskel& /* Library */);

    // Individual element parsers.
    //
    void
    Include_parser (::xml_schema::string_pskel&);

    void
    Library_parser (::xml_schema::string_pskel&);

    virtual void
    _reset ();

    // Constructor.
    //
    Directories_pskel ();

    // Implementation details.
    //
    protected:
    Directories_pskel* Directories_impl_;
    Directories_pskel (Directories_pskel*, void*);

    protected:
    virtual bool
    _start_element_impl (const ::xsde::cxx::ro_string&,
                         const ::xsde::cxx::ro_string&);

    virtual bool
    _end_element_impl (const ::xsde::cxx::ro_string&,
                       const ::xsde::cxx::ro_string&);

    protected:
    ::xml_schema::string_pskel* Include_parser_;
    ::xml_schema::string_pskel* Library_parser_;

    public:
    struct v_state_descr_
    {
      void (::egxml::Directories_pskel::*func) (
        unsigned long&,
        unsigned long&,
        const ::xsde::cxx::ro_string&,
        const ::xsde::cxx::ro_string&,
        bool);
      unsigned long state;
      unsigned long count;
    };

    struct v_state_
    {
      v_state_descr_ data[2UL];
      unsigned long size;
    };

    protected:
    v_state_ v_state_first_;
    ::xsde::cxx::stack v_state_stack_;

    virtual void
    _pre_e_validate ();

    virtual void
    _post_e_validate ();

    void
    sequence_0 (unsigned long&,
                unsigned long&,
                const ::xsde::cxx::ro_string&,
                const ::xsde::cxx::ro_string&,
                bool);
  };

  class Files_pskel: public ::xsde::cxx::parser::validating::complex_content
  {
    public:
    // Parser callbacks. Override them in your implementation.
    //
    // virtual void
    // pre ();

    // Elements.
    //
    virtual void
    System (const ::std::string&);

    virtual void
    User (const ::std::string&);

    virtual void
    Library (const ::std::string&);

    virtual ::egxml::Files*
    post_Files () = 0;

    // Parser construction API.
    //
    void
    parsers (::xml_schema::string_pskel& /* System */,
             ::xml_schema::string_pskel& /* User */,
             ::xml_schema::string_pskel& /* Library */);

    // Individual element parsers.
    //
    void
    System_parser (::xml_schema::string_pskel&);

    void
    User_parser (::xml_schema::string_pskel&);

    void
    Library_parser (::xml_schema::string_pskel&);

    virtual void
    _reset ();

    // Constructor.
    //
    Files_pskel ();

    // Implementation details.
    //
    protected:
    Files_pskel* Files_impl_;
    Files_pskel (Files_pskel*, void*);

    protected:
    virtual bool
    _start_element_impl (const ::xsde::cxx::ro_string&,
                         const ::xsde::cxx::ro_string&);

    virtual bool
    _end_element_impl (const ::xsde::cxx::ro_string&,
                       const ::xsde::cxx::ro_string&);

    protected:
    ::xml_schema::string_pskel* System_parser_;
    ::xml_schema::string_pskel* User_parser_;
    ::xml_schema::string_pskel* Library_parser_;

    public:
    struct v_state_descr_
    {
      void (::egxml::Files_pskel::*func) (
        unsigned long&,
        unsigned long&,
        const ::xsde::cxx::ro_string&,
        const ::xsde::cxx::ro_string&,
        bool);
      unsigned long state;
      unsigned long count;
    };

    struct v_state_
    {
      v_state_descr_ data[2UL];
      unsigned long size;
    };

    protected:
    v_state_ v_state_first_;
    ::xsde::cxx::stack v_state_stack_;

    virtual void
    _pre_e_validate ();

    virtual void
    _post_e_validate ();

    void
    sequence_0 (unsigned long&,
                unsigned long&,
                const ::xsde::cxx::ro_string&,
                const ::xsde::cxx::ro_string&,
                bool);
  };

  class Run_pskel: public ::xsde::cxx::parser::validating::complex_content
  {
    public:
    // Parser callbacks. Override them in your implementation.
    //
    // virtual void
    // pre ();

    // Elements.
    //
    virtual void
    Name (const ::std::string&);

    virtual void
    Argument (const ::std::string&);

    virtual ::egxml::Run*
    post_Run () = 0;

    // Parser construction API.
    //
    void
    parsers (::xml_schema::string_pskel& /* Name */,
             ::xml_schema::string_pskel& /* Argument */);

    // Individual element parsers.
    //
    void
    Name_parser (::xml_schema::string_pskel&);

    void
    Argument_parser (::xml_schema::string_pskel&);

    virtual void
    _reset ();

    // Constructor.
    //
    Run_pskel ();

    // Implementation details.
    //
    protected:
    Run_pskel* Run_impl_;
    Run_pskel (Run_pskel*, void*);

    protected:
    virtual bool
    _start_element_impl (const ::xsde::cxx::ro_string&,
                         const ::xsde::cxx::ro_string&);

    virtual bool
    _end_element_impl (const ::xsde::cxx::ro_string&,
                       const ::xsde::cxx::ro_string&);

    protected:
    ::xml_schema::string_pskel* Name_parser_;
    ::xml_schema::string_pskel* Argument_parser_;

    public:
    struct v_state_descr_
    {
      void (::egxml::Run_pskel::*func) (
        unsigned long&,
        unsigned long&,
        const ::xsde::cxx::ro_string&,
        const ::xsde::cxx::ro_string&,
        bool);
      unsigned long state;
      unsigned long count;
    };

    struct v_state_
    {
      v_state_descr_ data[2UL];
      unsigned long size;
    };

    protected:
    v_state_ v_state_first_;
    ::xsde::cxx::stack v_state_stack_;

    virtual void
    _pre_e_validate ();

    virtual void
    _post_e_validate ();

    void
    sequence_0 (unsigned long&,
                unsigned long&,
                const ::xsde::cxx::ro_string&,
                const ::xsde::cxx::ro_string&,
                bool);
  };
}

// Begin epilogue.
//
//
// End epilogue.

#include <xsde/cxx/post.hxx>

#endif // W__WORKSPACE_EG_SRC_DRIVER_SCHEMA_PSKEL_HXX
