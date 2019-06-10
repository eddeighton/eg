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

// Begin prologue.
//
//
// End prologue.

#include "schema-sskel.hxx"

namespace test
{
  // name_sskel
  //

  name_sskel::
  name_sskel (::xml_schema::string_sskel* tiein)
  : ::xml_schema::string_sskel (tiein, 0),
    name_impl_ (0)
  {
    this->_min_length_facet (1UL);
  }

  name_sskel::
  name_sskel (name_sskel* impl, void*)
  : ::xml_schema::string_sskel (impl, 0),
    name_impl_ (impl)
  {
    this->_min_length_facet (1UL);
  }

  // Config_sskel
  //

  void Config_sskel::
  name_serializer (::test::name_sskel& s)
  {
    this->name_serializer_ = &s;
  }

  void Config_sskel::
  file_serializer (::xml_schema::boolean_sskel& s)
  {
    this->file_serializer_ = &s;
  }

  void Config_sskel::
  serializers (::test::name_sskel& name,
               ::xml_schema::boolean_sskel& file)
  {
    this->name_serializer_ = &name;
    this->file_serializer_ = &file;
  }

  Config_sskel::
  Config_sskel ()
  : Config_impl_ (0),
    name_serializer_ (0),
    file_serializer_ (0)
  {
  }

  Config_sskel::
  Config_sskel (Config_sskel* impl, void*)
  : ::xsde::cxx::serializer::validating::complex_content (impl, 0),
    Config_impl_ (impl),
    name_serializer_ (0),
    file_serializer_ (0)
  {
  }

  // eg_sskel
  //

  void eg_sskel::
  host_serializer (::xml_schema::string_sskel& s)
  {
    this->host_serializer_ = &s;
  }

  void eg_sskel::
  folder_serializer (::xml_schema::string_sskel& s)
  {
    this->folder_serializer_ = &s;
  }

  void eg_sskel::
  stuff_serializer (::test::Config_sskel& s)
  {
    this->stuff_serializer_ = &s;
  }

  void eg_sskel::
  serializers (::xml_schema::string_sskel& host,
               ::xml_schema::string_sskel& folder,
               ::test::Config_sskel& stuff)
  {
    this->host_serializer_ = &host;
    this->folder_serializer_ = &folder;
    this->stuff_serializer_ = &stuff;
  }

  eg_sskel::
  eg_sskel ()
  : eg_impl_ (0),
    host_serializer_ (0),
    folder_serializer_ (0),
    stuff_serializer_ (0)
  {
  }

  eg_sskel::
  eg_sskel (eg_sskel* impl, void*)
  : ::xsde::cxx::serializer::validating::complex_content (impl, 0),
    eg_impl_ (impl),
    host_serializer_ (0),
    folder_serializer_ (0),
    stuff_serializer_ (0)
  {
  }
}

#include <assert.h>

namespace test
{
  // name_sskel
  //

  void name_sskel::
  pre (const ::std::string& x)
  {
    assert (this->string_impl_);
    this->string_impl_->pre (x);
  }

  // Config_sskel
  //

  void Config_sskel::
  _reset ()
  {
    if (this->resetting_)
      return;

    typedef ::xsde::cxx::serializer::validating::complex_content base;
    base::_reset ();

    this->resetting_ = true;

    if (this->name_serializer_)
      this->name_serializer_->_reset ();

    if (this->file_serializer_)
      this->file_serializer_->_reset ();

    this->resetting_ = false;
  }

  // eg_sskel
  //

  bool eg_sskel::
  stuff_next ()
  {
    return this->eg_impl_ ? this->eg_impl_->stuff_next () : false;
  }

  void eg_sskel::
  _reset ()
  {
    if (this->resetting_)
      return;

    typedef ::xsde::cxx::serializer::validating::complex_content base;
    base::_reset ();

    this->resetting_ = true;

    if (this->host_serializer_)
      this->host_serializer_->_reset ();

    if (this->folder_serializer_)
      this->folder_serializer_->_reset ();

    if (this->stuff_serializer_)
      this->stuff_serializer_->_reset ();

    this->resetting_ = false;
  }
}

namespace test
{
  // Element validation and serialization for Config_sskel.
  //
  void Config_sskel::
  _serialize_content ()
  {
    ::xsde::cxx::serializer::context& ctx = this->_context ();

    // name
    //
    {
      const ::test::name& r = this->name ();

      if (this->name_serializer_)
      {
        this->name_serializer_->pre (r);
        this->_start_element ("name");
        this->name_serializer_->_pre_impl (ctx);

        if (ctx.error_type ())
          return;

        this->name_serializer_->_serialize_attributes ();

        if (ctx.error_type ())
          return;

        this->name_serializer_->_serialize_content ();

        if (ctx.error_type ())
          return;

        this->name_serializer_->_post_impl ();

        if (ctx.error_type ())
          return;

        this->_end_element ();
        this->name_serializer_->post ();
      }
      else
      {
        this->_schema_error (::xsde::cxx::schema_error::expected_element);
        return;
      }
    }

    // file
    //
    {
      bool r = this->file ();

      if (this->file_serializer_)
      {
        this->file_serializer_->pre (r);
        this->_start_element ("file");
        this->file_serializer_->_pre_impl (ctx);

        if (ctx.error_type ())
          return;

        this->file_serializer_->_serialize_attributes ();

        if (ctx.error_type ())
          return;

        this->file_serializer_->_serialize_content ();

        if (ctx.error_type ())
          return;

        this->file_serializer_->_post_impl ();

        if (ctx.error_type ())
          return;

        this->_end_element ();
        this->file_serializer_->post ();
      }
      else
      {
        this->_schema_error (::xsde::cxx::schema_error::expected_element);
        return;
      }
    }
  }

  // Element validation and serialization for eg_sskel.
  //
  void eg_sskel::
  _serialize_content ()
  {
    ::xsde::cxx::serializer::context& ctx = this->_context ();

    // host
    //
    {
      const ::std::string& r = this->host ();

      if (this->host_serializer_)
      {
        this->host_serializer_->pre (r);
        this->_start_element ("host");
        this->host_serializer_->_pre_impl (ctx);

        if (ctx.error_type ())
          return;

        this->host_serializer_->_serialize_attributes ();

        if (ctx.error_type ())
          return;

        this->host_serializer_->_serialize_content ();

        if (ctx.error_type ())
          return;

        this->host_serializer_->_post_impl ();

        if (ctx.error_type ())
          return;

        this->_end_element ();
        this->host_serializer_->post ();
      }
      else
      {
        this->_schema_error (::xsde::cxx::schema_error::expected_element);
        return;
      }
    }

    // folder
    //
    {
      const ::std::string& r = this->folder ();

      if (this->folder_serializer_)
      {
        this->folder_serializer_->pre (r);
        this->_start_element ("folder");
        this->folder_serializer_->_pre_impl (ctx);

        if (ctx.error_type ())
          return;

        this->folder_serializer_->_serialize_attributes ();

        if (ctx.error_type ())
          return;

        this->folder_serializer_->_serialize_content ();

        if (ctx.error_type ())
          return;

        this->folder_serializer_->_post_impl ();

        if (ctx.error_type ())
          return;

        this->_end_element ();
        this->folder_serializer_->post ();
      }
      else
      {
        this->_schema_error (::xsde::cxx::schema_error::expected_element);
        return;
      }
    }

    // stuff
    //
    while (this->stuff_next ())
    {
      const ::test::Config& r = this->stuff ();

      if (this->stuff_serializer_)
      {
        this->stuff_serializer_->pre (r);
        this->_start_element ("stuff");
        this->stuff_serializer_->_pre_impl (ctx);

        if (ctx.error_type ())
          return;

        this->stuff_serializer_->_serialize_attributes ();

        if (ctx.error_type ())
          return;

        this->stuff_serializer_->_serialize_content ();

        if (ctx.error_type ())
          return;

        this->stuff_serializer_->_post_impl ();

        if (ctx.error_type ())
          return;

        this->_end_element ();
        this->stuff_serializer_->post ();
      }
    }
  }
}

namespace test
{
}

// Begin epilogue.
//
//
// End epilogue.

