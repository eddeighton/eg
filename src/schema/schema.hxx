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

#ifndef W__WORKSPACE_EG_SRC_SCHEMA_SCHEMA_HXX
#define W__WORKSPACE_EG_SRC_SCHEMA_SCHEMA_HXX

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

#ifndef XSDE_EXCEPTIONS
#error the generated code uses exceptions while the XSD/e runtime does not (reconfigure the runtime or add --no-exceptions)
#endif

#ifdef XSDE_LONGLONG
#error the XSD/e runtime uses long long while the generated code does not (reconfigure the runtime or remove --no-long-long)
#endif

#ifdef XSDE_CUSTOM_ALLOCATOR
#error the XSD/e runtime uses custom allocator while the generated code does not (reconfigure the runtime or add --custom-allocator)
#endif

#include <xsde/cxx/pre.hxx>

// Begin prologue.
//
//
// End prologue.

#include <string>
#include <xsde/cxx/hybrid/xml-schema.hxx>
#include <xsde/cxx/hybrid/sequence.hxx>

namespace xml_schema
{
  using ::xsde::cxx::hybrid::any_type;
  typedef ::std::string any_simple_type;

  typedef signed char byte;
  using ::xsde::cxx::hybrid::byte_base;

  typedef unsigned char unsigned_byte;
  using ::xsde::cxx::hybrid::unsigned_byte_base;

  typedef short short_;
  using ::xsde::cxx::hybrid::short_base;

  typedef unsigned short unsigned_short;
  using ::xsde::cxx::hybrid::unsigned_short_base;

  typedef int int_;
  using ::xsde::cxx::hybrid::int_base;

  typedef unsigned int unsigned_int;
  using ::xsde::cxx::hybrid::unsigned_int_base;

  typedef long long_;
  using ::xsde::cxx::hybrid::long_base;

  typedef unsigned long unsigned_long;
  using ::xsde::cxx::hybrid::unsigned_long_base;

  typedef long integer;
  using ::xsde::cxx::hybrid::integer_base;

  typedef long non_positive_integer;
  using ::xsde::cxx::hybrid::non_positive_integer_base;

  typedef unsigned long non_negative_integer;
  using ::xsde::cxx::hybrid::non_negative_integer_base;

  typedef unsigned long positive_integer;
  using ::xsde::cxx::hybrid::positive_integer_base;

  typedef long negative_integer;
  using ::xsde::cxx::hybrid::negative_integer_base;

  typedef bool boolean;
  using ::xsde::cxx::hybrid::boolean_base;

  typedef float float_;
  using ::xsde::cxx::hybrid::float_base;

  typedef double double_;
  using ::xsde::cxx::hybrid::double_base;

  typedef double decimal;
  using ::xsde::cxx::hybrid::decimal_base;

  typedef ::std::string string;

  typedef ::std::string normalized_string;

  typedef ::std::string token;

  typedef ::std::string name;

  typedef ::std::string nmtoken;

  typedef ::xsde::cxx::string_sequence nmtokens;

  typedef ::std::string ncname;

  typedef ::std::string language;

  typedef ::std::string id;

  typedef ::std::string idref;

  typedef ::xsde::cxx::string_sequence idrefs;

  typedef ::std::string uri;

  using ::xsde::cxx::qname;

  using ::xsde::cxx::buffer;
  typedef ::xsde::cxx::buffer base64_binary;
  typedef ::xsde::cxx::buffer hex_binary;

  using ::xsde::cxx::time_zone;
  using ::xsde::cxx::date;
  using ::xsde::cxx::date_time;
  using ::xsde::cxx::duration;
  using ::xsde::cxx::gday;
  using ::xsde::cxx::gmonth;
  using ::xsde::cxx::gmonth_day;
  using ::xsde::cxx::gyear;
  using ::xsde::cxx::gyear_month;
  using ::xsde::cxx::time;

  using ::xsde::cxx::hybrid::pod_sequence;
  using ::xsde::cxx::hybrid::fix_sequence;
  using ::xsde::cxx::hybrid::var_sequence;
  using ::xsde::cxx::string_sequence;
  using ::xsde::cxx::hybrid::data_sequence;
}

namespace test
{
  class name;
  class Config;
  class eg;
}


namespace test
{
  // name (fixed-length)
  //
  class name: public ::std::string
  {
    public:
    name ();
  };

  // Config (fixed-length)
  //
  class Config
  {
    public:
    Config ();

    Config (const Config&);
    Config& operator= (const Config&);

    ~Config ();

    // name
    //
    const ::test::name&
    name () const;

    ::test::name&
    name ();

    void
    name (const ::test::name&);

    // file
    //
    bool
    file () const;

    bool&
    file ();

    void
    file (bool);

    private:
    ::test::name name_;
    bool file_;
  };

  // eg (variable-length)
  //
  class eg
  {
    private:
    eg (const eg&);
    eg& operator= (const eg&);

    public:
    eg ();

    ~eg ();

    // host
    //
    const ::std::string&
    host () const;

    ::std::string&
    host ();

    void
    host (const ::std::string&);

    // folder
    //
    const ::std::string&
    folder () const;

    ::std::string&
    folder ();

    void
    folder (const ::std::string&);

    // stuff
    //
    typedef ::xsde::cxx::hybrid::fix_sequence< ::test::Config > stuff_sequence;
    typedef stuff_sequence::iterator stuff_iterator;
    typedef stuff_sequence::const_iterator stuff_const_iterator;

    const stuff_sequence&
    stuff () const;

    stuff_sequence&
    stuff ();

    private:
    ::std::string host_;
    ::std::string folder_;
    stuff_sequence stuff_;
  };
}

// Begin epilogue.
//
//
// End epilogue.

#include <xsde/cxx/post.hxx>

#endif // W__WORKSPACE_EG_SRC_SCHEMA_SCHEMA_HXX
