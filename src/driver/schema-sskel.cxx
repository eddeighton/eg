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

// Package_sskel
//

void Package_sskel::
Name_serializer (::xml_schema::string_sskel& s)
{
  this->Name_serializer_ = &s;
}

void Package_sskel::
Repository_serializer (::xml_schema::string_sskel& s)
{
  this->Repository_serializer_ = &s;
}

void Package_sskel::
License_serializer (::xml_schema::string_sskel& s)
{
  this->License_serializer_ = &s;
}

void Package_sskel::
Description_serializer (::xml_schema::string_sskel& s)
{
  this->Description_serializer_ = &s;
}

void Package_sskel::
Directories_serializer (::Directories_sskel& s)
{
  this->Directories_serializer_ = &s;
}

void Package_sskel::
Files_serializer (::Files_sskel& s)
{
  this->Files_serializer_ = &s;
}

void Package_sskel::
serializers (::xml_schema::string_sskel& Name,
             ::xml_schema::string_sskel& Repository,
             ::xml_schema::string_sskel& License,
             ::xml_schema::string_sskel& Description,
             ::Directories_sskel& Directories,
             ::Files_sskel& Files)
{
  this->Name_serializer_ = &Name;
  this->Repository_serializer_ = &Repository;
  this->License_serializer_ = &License;
  this->Description_serializer_ = &Description;
  this->Directories_serializer_ = &Directories;
  this->Files_serializer_ = &Files;
}

Package_sskel::
Package_sskel ()
: Package_impl_ (0),
  Name_serializer_ (0),
  Repository_serializer_ (0),
  License_serializer_ (0),
  Description_serializer_ (0),
  Directories_serializer_ (0),
  Files_serializer_ (0)
{
}

Package_sskel::
Package_sskel (Package_sskel* impl, void*)
: ::xsde::cxx::serializer::validating::complex_content (impl, 0),
  Package_impl_ (impl),
  Name_serializer_ (0),
  Repository_serializer_ (0),
  License_serializer_ (0),
  Description_serializer_ (0),
  Directories_serializer_ (0),
  Files_serializer_ (0)
{
}

// Host_sskel
//

void Host_sskel::
Name_serializer (::xml_schema::string_sskel& s)
{
  this->Name_serializer_ = &s;
}

void Host_sskel::
Command_serializer (::xml_schema::string_sskel& s)
{
  this->Command_serializer_ = &s;
}

void Host_sskel::
Repository_serializer (::xml_schema::string_sskel& s)
{
  this->Repository_serializer_ = &s;
}

void Host_sskel::
License_serializer (::xml_schema::string_sskel& s)
{
  this->License_serializer_ = &s;
}

void Host_sskel::
Description_serializer (::xml_schema::string_sskel& s)
{
  this->Description_serializer_ = &s;
}

void Host_sskel::
Directories_serializer (::Directories1_sskel& s)
{
  this->Directories_serializer_ = &s;
}

void Host_sskel::
Files_serializer (::Files1_sskel& s)
{
  this->Files_serializer_ = &s;
}

void Host_sskel::
serializers (::xml_schema::string_sskel& Name,
             ::xml_schema::string_sskel& Command,
             ::xml_schema::string_sskel& Repository,
             ::xml_schema::string_sskel& License,
             ::xml_schema::string_sskel& Description,
             ::Directories1_sskel& Directories,
             ::Files1_sskel& Files)
{
  this->Name_serializer_ = &Name;
  this->Command_serializer_ = &Command;
  this->Repository_serializer_ = &Repository;
  this->License_serializer_ = &License;
  this->Description_serializer_ = &Description;
  this->Directories_serializer_ = &Directories;
  this->Files_serializer_ = &Files;
}

Host_sskel::
Host_sskel ()
: Host_impl_ (0),
  Name_serializer_ (0),
  Command_serializer_ (0),
  Repository_serializer_ (0),
  License_serializer_ (0),
  Description_serializer_ (0),
  Directories_serializer_ (0),
  Files_serializer_ (0)
{
}

Host_sskel::
Host_sskel (Host_sskel* impl, void*)
: ::xsde::cxx::serializer::validating::complex_content (impl, 0),
  Host_impl_ (impl),
  Name_serializer_ (0),
  Command_serializer_ (0),
  Repository_serializer_ (0),
  License_serializer_ (0),
  Description_serializer_ (0),
  Directories_serializer_ (0),
  Files_serializer_ (0)
{
}

// EG_sskel
//

void EG_sskel::
Package_serializer (::Package_sskel& s)
{
  this->Package_serializer_ = &s;
}

void EG_sskel::
Host_serializer (::Host_sskel& s)
{
  this->Host_serializer_ = &s;
}

void EG_sskel::
serializers (::Package_sskel& Package,
             ::Host_sskel& Host)
{
  this->Package_serializer_ = &Package;
  this->Host_serializer_ = &Host;
}

EG_sskel::
EG_sskel ()
: EG_impl_ (0),
  Package_serializer_ (0),
  Host_serializer_ (0)
{
}

EG_sskel::
EG_sskel (EG_sskel* impl, void*)
: ::xsde::cxx::serializer::validating::complex_content (impl, 0),
  EG_impl_ (impl),
  Package_serializer_ (0),
  Host_serializer_ (0)
{
}

// Directories_sskel
//

void Directories_sskel::
Include_serializer (::xml_schema::string_sskel& s)
{
  this->Include_serializer_ = &s;
}

void Directories_sskel::
Library_serializer (::xml_schema::string_sskel& s)
{
  this->Library_serializer_ = &s;
}

void Directories_sskel::
serializers (::xml_schema::string_sskel& Include,
             ::xml_schema::string_sskel& Library)
{
  this->Include_serializer_ = &Include;
  this->Library_serializer_ = &Library;
}

Directories_sskel::
Directories_sskel ()
: Directories_impl_ (0),
  Include_serializer_ (0),
  Library_serializer_ (0)
{
}

Directories_sskel::
Directories_sskel (Directories_sskel* impl, void*)
: ::xsde::cxx::serializer::validating::complex_content (impl, 0),
  Directories_impl_ (impl),
  Include_serializer_ (0),
  Library_serializer_ (0)
{
}

// Files_sskel
//

void Files_sskel::
Include_serializer (::xml_schema::string_sskel& s)
{
  this->Include_serializer_ = &s;
}

void Files_sskel::
Library_serializer (::xml_schema::string_sskel& s)
{
  this->Library_serializer_ = &s;
}

void Files_sskel::
serializers (::xml_schema::string_sskel& Include,
             ::xml_schema::string_sskel& Library)
{
  this->Include_serializer_ = &Include;
  this->Library_serializer_ = &Library;
}

Files_sskel::
Files_sskel ()
: Files_impl_ (0),
  Include_serializer_ (0),
  Library_serializer_ (0)
{
}

Files_sskel::
Files_sskel (Files_sskel* impl, void*)
: ::xsde::cxx::serializer::validating::complex_content (impl, 0),
  Files_impl_ (impl),
  Include_serializer_ (0),
  Library_serializer_ (0)
{
}

// Directories1_sskel
//

void Directories1_sskel::
Include_serializer (::xml_schema::string_sskel& s)
{
  this->Include_serializer_ = &s;
}

void Directories1_sskel::
Library_serializer (::xml_schema::string_sskel& s)
{
  this->Library_serializer_ = &s;
}

void Directories1_sskel::
serializers (::xml_schema::string_sskel& Include,
             ::xml_schema::string_sskel& Library)
{
  this->Include_serializer_ = &Include;
  this->Library_serializer_ = &Library;
}

Directories1_sskel::
Directories1_sskel ()
: Directories1_impl_ (0),
  Include_serializer_ (0),
  Library_serializer_ (0)
{
}

Directories1_sskel::
Directories1_sskel (Directories1_sskel* impl, void*)
: ::xsde::cxx::serializer::validating::complex_content (impl, 0),
  Directories1_impl_ (impl),
  Include_serializer_ (0),
  Library_serializer_ (0)
{
}

// Files1_sskel
//

void Files1_sskel::
Include_serializer (::xml_schema::string_sskel& s)
{
  this->Include_serializer_ = &s;
}

void Files1_sskel::
Library_serializer (::xml_schema::string_sskel& s)
{
  this->Library_serializer_ = &s;
}

void Files1_sskel::
serializers (::xml_schema::string_sskel& Include,
             ::xml_schema::string_sskel& Library)
{
  this->Include_serializer_ = &Include;
  this->Library_serializer_ = &Library;
}

Files1_sskel::
Files1_sskel ()
: Files1_impl_ (0),
  Include_serializer_ (0),
  Library_serializer_ (0)
{
}

Files1_sskel::
Files1_sskel (Files1_sskel* impl, void*)
: ::xsde::cxx::serializer::validating::complex_content (impl, 0),
  Files1_impl_ (impl),
  Include_serializer_ (0),
  Library_serializer_ (0)
{
}

#include <assert.h>

// Package_sskel
//

bool Package_sskel::
Repository_present ()
{
  return this->Package_impl_ ? this->Package_impl_->Repository_present () : false;
}

bool Package_sskel::
Description_present ()
{
  return this->Package_impl_ ? this->Package_impl_->Description_present () : false;
}

bool Package_sskel::
Directories_present ()
{
  return this->Package_impl_ ? this->Package_impl_->Directories_present () : false;
}

bool Package_sskel::
Files_present ()
{
  return this->Package_impl_ ? this->Package_impl_->Files_present () : false;
}

void Package_sskel::
_reset ()
{
  if (this->resetting_)
    return;

  typedef ::xsde::cxx::serializer::validating::complex_content base;
  base::_reset ();

  this->resetting_ = true;

  if (this->Name_serializer_)
    this->Name_serializer_->_reset ();

  if (this->Repository_serializer_)
    this->Repository_serializer_->_reset ();

  if (this->License_serializer_)
    this->License_serializer_->_reset ();

  if (this->Description_serializer_)
    this->Description_serializer_->_reset ();

  if (this->Directories_serializer_)
    this->Directories_serializer_->_reset ();

  if (this->Files_serializer_)
    this->Files_serializer_->_reset ();

  this->resetting_ = false;
}

// Host_sskel
//

bool Host_sskel::
Repository_present ()
{
  return this->Host_impl_ ? this->Host_impl_->Repository_present () : false;
}

bool Host_sskel::
Description_present ()
{
  return this->Host_impl_ ? this->Host_impl_->Description_present () : false;
}

bool Host_sskel::
Directories_present ()
{
  return this->Host_impl_ ? this->Host_impl_->Directories_present () : false;
}

bool Host_sskel::
Files_present ()
{
  return this->Host_impl_ ? this->Host_impl_->Files_present () : false;
}

void Host_sskel::
_reset ()
{
  if (this->resetting_)
    return;

  typedef ::xsde::cxx::serializer::validating::complex_content base;
  base::_reset ();

  this->resetting_ = true;

  if (this->Name_serializer_)
    this->Name_serializer_->_reset ();

  if (this->Command_serializer_)
    this->Command_serializer_->_reset ();

  if (this->Repository_serializer_)
    this->Repository_serializer_->_reset ();

  if (this->License_serializer_)
    this->License_serializer_->_reset ();

  if (this->Description_serializer_)
    this->Description_serializer_->_reset ();

  if (this->Directories_serializer_)
    this->Directories_serializer_->_reset ();

  if (this->Files_serializer_)
    this->Files_serializer_->_reset ();

  this->resetting_ = false;
}

// EG_sskel
//

void EG_sskel::
_reset ()
{
  if (this->resetting_)
    return;

  typedef ::xsde::cxx::serializer::validating::complex_content base;
  base::_reset ();

  this->resetting_ = true;

  if (this->Package_serializer_)
    this->Package_serializer_->_reset ();

  if (this->Host_serializer_)
    this->Host_serializer_->_reset ();

  this->resetting_ = false;
}

// Directories_sskel
//

bool Directories_sskel::
Include_next ()
{
  return this->Directories_impl_ ? this->Directories_impl_->Include_next () : false;
}

bool Directories_sskel::
Library_next ()
{
  return this->Directories_impl_ ? this->Directories_impl_->Library_next () : false;
}

void Directories_sskel::
_reset ()
{
  if (this->resetting_)
    return;

  typedef ::xsde::cxx::serializer::validating::complex_content base;
  base::_reset ();

  this->resetting_ = true;

  if (this->Include_serializer_)
    this->Include_serializer_->_reset ();

  if (this->Library_serializer_)
    this->Library_serializer_->_reset ();

  this->resetting_ = false;
}

// Files_sskel
//

bool Files_sskel::
Include_next ()
{
  return this->Files_impl_ ? this->Files_impl_->Include_next () : false;
}

bool Files_sskel::
Library_next ()
{
  return this->Files_impl_ ? this->Files_impl_->Library_next () : false;
}

void Files_sskel::
_reset ()
{
  if (this->resetting_)
    return;

  typedef ::xsde::cxx::serializer::validating::complex_content base;
  base::_reset ();

  this->resetting_ = true;

  if (this->Include_serializer_)
    this->Include_serializer_->_reset ();

  if (this->Library_serializer_)
    this->Library_serializer_->_reset ();

  this->resetting_ = false;
}

// Directories1_sskel
//

bool Directories1_sskel::
Include_next ()
{
  return this->Directories1_impl_ ? this->Directories1_impl_->Include_next () : false;
}

bool Directories1_sskel::
Library_next ()
{
  return this->Directories1_impl_ ? this->Directories1_impl_->Library_next () : false;
}

void Directories1_sskel::
_reset ()
{
  if (this->resetting_)
    return;

  typedef ::xsde::cxx::serializer::validating::complex_content base;
  base::_reset ();

  this->resetting_ = true;

  if (this->Include_serializer_)
    this->Include_serializer_->_reset ();

  if (this->Library_serializer_)
    this->Library_serializer_->_reset ();

  this->resetting_ = false;
}

// Files1_sskel
//

bool Files1_sskel::
Include_next ()
{
  return this->Files1_impl_ ? this->Files1_impl_->Include_next () : false;
}

bool Files1_sskel::
Library_next ()
{
  return this->Files1_impl_ ? this->Files1_impl_->Library_next () : false;
}

void Files1_sskel::
_reset ()
{
  if (this->resetting_)
    return;

  typedef ::xsde::cxx::serializer::validating::complex_content base;
  base::_reset ();

  this->resetting_ = true;

  if (this->Include_serializer_)
    this->Include_serializer_->_reset ();

  if (this->Library_serializer_)
    this->Library_serializer_->_reset ();

  this->resetting_ = false;
}

// Element validation and serialization for Package_sskel.
//
void Package_sskel::
_serialize_content ()
{
  ::xsde::cxx::serializer::context& ctx = this->_context ();

  // Name
  //
  {
    const ::std::string& r = this->Name ();

    if (this->Name_serializer_)
    {
      this->Name_serializer_->pre (r);
      this->_start_element ("Name");
      this->Name_serializer_->_pre_impl (ctx);

      if (ctx.error_type ())
        return;

      this->Name_serializer_->_serialize_attributes ();

      if (ctx.error_type ())
        return;

      this->Name_serializer_->_serialize_content ();

      if (ctx.error_type ())
        return;

      this->Name_serializer_->_post_impl ();

      if (ctx.error_type ())
        return;

      this->_end_element ();
      this->Name_serializer_->post ();
    }
    else
    {
      this->_schema_error (::xsde::cxx::schema_error::expected_element);
      return;
    }
  }

  // Repository
  //
  if (this->Repository_present ())
  {
    const ::std::string& r = this->Repository ();

    if (this->Repository_serializer_)
    {
      this->Repository_serializer_->pre (r);
      this->_start_element ("Repository");
      this->Repository_serializer_->_pre_impl (ctx);

      if (ctx.error_type ())
        return;

      this->Repository_serializer_->_serialize_attributes ();

      if (ctx.error_type ())
        return;

      this->Repository_serializer_->_serialize_content ();

      if (ctx.error_type ())
        return;

      this->Repository_serializer_->_post_impl ();

      if (ctx.error_type ())
        return;

      this->_end_element ();
      this->Repository_serializer_->post ();
    }
  }

  // License
  //
  {
    const ::std::string& r = this->License ();

    if (this->License_serializer_)
    {
      this->License_serializer_->pre (r);
      this->_start_element ("License");
      this->License_serializer_->_pre_impl (ctx);

      if (ctx.error_type ())
        return;

      this->License_serializer_->_serialize_attributes ();

      if (ctx.error_type ())
        return;

      this->License_serializer_->_serialize_content ();

      if (ctx.error_type ())
        return;

      this->License_serializer_->_post_impl ();

      if (ctx.error_type ())
        return;

      this->_end_element ();
      this->License_serializer_->post ();
    }
    else
    {
      this->_schema_error (::xsde::cxx::schema_error::expected_element);
      return;
    }
  }

  // Description
  //
  if (this->Description_present ())
  {
    const ::std::string& r = this->Description ();

    if (this->Description_serializer_)
    {
      this->Description_serializer_->pre (r);
      this->_start_element ("Description");
      this->Description_serializer_->_pre_impl (ctx);

      if (ctx.error_type ())
        return;

      this->Description_serializer_->_serialize_attributes ();

      if (ctx.error_type ())
        return;

      this->Description_serializer_->_serialize_content ();

      if (ctx.error_type ())
        return;

      this->Description_serializer_->_post_impl ();

      if (ctx.error_type ())
        return;

      this->_end_element ();
      this->Description_serializer_->post ();
    }
  }

  // Directories
  //
  if (this->Directories_present ())
  {
    const ::Directories& r = this->Directories ();

    if (this->Directories_serializer_)
    {
      this->Directories_serializer_->pre (r);
      this->_start_element ("Directories");
      this->Directories_serializer_->_pre_impl (ctx);

      if (ctx.error_type ())
        return;

      this->Directories_serializer_->_serialize_attributes ();

      if (ctx.error_type ())
        return;

      this->Directories_serializer_->_serialize_content ();

      if (ctx.error_type ())
        return;

      this->Directories_serializer_->_post_impl ();

      if (ctx.error_type ())
        return;

      this->_end_element ();
      this->Directories_serializer_->post ();
    }
  }

  // Files
  //
  if (this->Files_present ())
  {
    const ::Files& r = this->Files ();

    if (this->Files_serializer_)
    {
      this->Files_serializer_->pre (r);
      this->_start_element ("Files");
      this->Files_serializer_->_pre_impl (ctx);

      if (ctx.error_type ())
        return;

      this->Files_serializer_->_serialize_attributes ();

      if (ctx.error_type ())
        return;

      this->Files_serializer_->_serialize_content ();

      if (ctx.error_type ())
        return;

      this->Files_serializer_->_post_impl ();

      if (ctx.error_type ())
        return;

      this->_end_element ();
      this->Files_serializer_->post ();
    }
  }
}

// Element validation and serialization for Host_sskel.
//
void Host_sskel::
_serialize_content ()
{
  ::xsde::cxx::serializer::context& ctx = this->_context ();

  // Name
  //
  {
    const ::std::string& r = this->Name ();

    if (this->Name_serializer_)
    {
      this->Name_serializer_->pre (r);
      this->_start_element ("Name");
      this->Name_serializer_->_pre_impl (ctx);

      if (ctx.error_type ())
        return;

      this->Name_serializer_->_serialize_attributes ();

      if (ctx.error_type ())
        return;

      this->Name_serializer_->_serialize_content ();

      if (ctx.error_type ())
        return;

      this->Name_serializer_->_post_impl ();

      if (ctx.error_type ())
        return;

      this->_end_element ();
      this->Name_serializer_->post ();
    }
    else
    {
      this->_schema_error (::xsde::cxx::schema_error::expected_element);
      return;
    }
  }

  // Command
  //
  {
    const ::std::string& r = this->Command ();

    if (this->Command_serializer_)
    {
      this->Command_serializer_->pre (r);
      this->_start_element ("Command");
      this->Command_serializer_->_pre_impl (ctx);

      if (ctx.error_type ())
        return;

      this->Command_serializer_->_serialize_attributes ();

      if (ctx.error_type ())
        return;

      this->Command_serializer_->_serialize_content ();

      if (ctx.error_type ())
        return;

      this->Command_serializer_->_post_impl ();

      if (ctx.error_type ())
        return;

      this->_end_element ();
      this->Command_serializer_->post ();
    }
    else
    {
      this->_schema_error (::xsde::cxx::schema_error::expected_element);
      return;
    }
  }

  // Repository
  //
  if (this->Repository_present ())
  {
    const ::std::string& r = this->Repository ();

    if (this->Repository_serializer_)
    {
      this->Repository_serializer_->pre (r);
      this->_start_element ("Repository");
      this->Repository_serializer_->_pre_impl (ctx);

      if (ctx.error_type ())
        return;

      this->Repository_serializer_->_serialize_attributes ();

      if (ctx.error_type ())
        return;

      this->Repository_serializer_->_serialize_content ();

      if (ctx.error_type ())
        return;

      this->Repository_serializer_->_post_impl ();

      if (ctx.error_type ())
        return;

      this->_end_element ();
      this->Repository_serializer_->post ();
    }
  }

  // License
  //
  {
    const ::std::string& r = this->License ();

    if (this->License_serializer_)
    {
      this->License_serializer_->pre (r);
      this->_start_element ("License");
      this->License_serializer_->_pre_impl (ctx);

      if (ctx.error_type ())
        return;

      this->License_serializer_->_serialize_attributes ();

      if (ctx.error_type ())
        return;

      this->License_serializer_->_serialize_content ();

      if (ctx.error_type ())
        return;

      this->License_serializer_->_post_impl ();

      if (ctx.error_type ())
        return;

      this->_end_element ();
      this->License_serializer_->post ();
    }
    else
    {
      this->_schema_error (::xsde::cxx::schema_error::expected_element);
      return;
    }
  }

  // Description
  //
  if (this->Description_present ())
  {
    const ::std::string& r = this->Description ();

    if (this->Description_serializer_)
    {
      this->Description_serializer_->pre (r);
      this->_start_element ("Description");
      this->Description_serializer_->_pre_impl (ctx);

      if (ctx.error_type ())
        return;

      this->Description_serializer_->_serialize_attributes ();

      if (ctx.error_type ())
        return;

      this->Description_serializer_->_serialize_content ();

      if (ctx.error_type ())
        return;

      this->Description_serializer_->_post_impl ();

      if (ctx.error_type ())
        return;

      this->_end_element ();
      this->Description_serializer_->post ();
    }
  }

  // Directories
  //
  if (this->Directories_present ())
  {
    const ::Directories1& r = this->Directories ();

    if (this->Directories_serializer_)
    {
      this->Directories_serializer_->pre (r);
      this->_start_element ("Directories");
      this->Directories_serializer_->_pre_impl (ctx);

      if (ctx.error_type ())
        return;

      this->Directories_serializer_->_serialize_attributes ();

      if (ctx.error_type ())
        return;

      this->Directories_serializer_->_serialize_content ();

      if (ctx.error_type ())
        return;

      this->Directories_serializer_->_post_impl ();

      if (ctx.error_type ())
        return;

      this->_end_element ();
      this->Directories_serializer_->post ();
    }
  }

  // Files
  //
  if (this->Files_present ())
  {
    const ::Files1& r = this->Files ();

    if (this->Files_serializer_)
    {
      this->Files_serializer_->pre (r);
      this->_start_element ("Files");
      this->Files_serializer_->_pre_impl (ctx);

      if (ctx.error_type ())
        return;

      this->Files_serializer_->_serialize_attributes ();

      if (ctx.error_type ())
        return;

      this->Files_serializer_->_serialize_content ();

      if (ctx.error_type ())
        return;

      this->Files_serializer_->_post_impl ();

      if (ctx.error_type ())
        return;

      this->_end_element ();
      this->Files_serializer_->post ();
    }
  }
}

// Element validation and serialization for EG_sskel.
//
void EG_sskel::
_serialize_content ()
{
  ::xsde::cxx::serializer::context& ctx = this->_context ();

  switch (this->choice_arm ())
  {
    case Package_tag:
    {
      // Package
      //
      {
        const ::Package& r = this->Package ();

        if (this->Package_serializer_)
        {
          this->Package_serializer_->pre (r);
          this->_start_element ("Package");
          this->Package_serializer_->_pre_impl (ctx);

          if (ctx.error_type ())
            return;

          this->Package_serializer_->_serialize_attributes ();

          if (ctx.error_type ())
            return;

          this->Package_serializer_->_serialize_content ();

          if (ctx.error_type ())
            return;

          this->Package_serializer_->_post_impl ();

          if (ctx.error_type ())
            return;

          this->_end_element ();
          this->Package_serializer_->post ();
        }
        else
        {
          this->_schema_error (::xsde::cxx::schema_error::expected_element);
          return;
        }
      }

      break;
    }
    case Host_tag:
    {
      // Host
      //
      {
        const ::Host& r = this->Host ();

        if (this->Host_serializer_)
        {
          this->Host_serializer_->pre (r);
          this->_start_element ("Host");
          this->Host_serializer_->_pre_impl (ctx);

          if (ctx.error_type ())
            return;

          this->Host_serializer_->_serialize_attributes ();

          if (ctx.error_type ())
            return;

          this->Host_serializer_->_serialize_content ();

          if (ctx.error_type ())
            return;

          this->Host_serializer_->_post_impl ();

          if (ctx.error_type ())
            return;

          this->_end_element ();
          this->Host_serializer_->post ();
        }
        else
        {
          this->_schema_error (::xsde::cxx::schema_error::expected_element);
          return;
        }
      }

      break;
    }
    default:
    {
      this->_schema_error (::xsde::cxx::schema_error::unexpected_element);
      return;
    }
  }
}

// Element validation and serialization for Directories_sskel.
//
void Directories_sskel::
_serialize_content ()
{
  ::xsde::cxx::serializer::context& ctx = this->_context ();

  // Include
  //
  while (this->Include_next ())
  {
    const ::std::string& r = this->Include ();

    if (this->Include_serializer_)
    {
      this->Include_serializer_->pre (r);
      this->_start_element ("Include");
      this->Include_serializer_->_pre_impl (ctx);

      if (ctx.error_type ())
        return;

      this->Include_serializer_->_serialize_attributes ();

      if (ctx.error_type ())
        return;

      this->Include_serializer_->_serialize_content ();

      if (ctx.error_type ())
        return;

      this->Include_serializer_->_post_impl ();

      if (ctx.error_type ())
        return;

      this->_end_element ();
      this->Include_serializer_->post ();
    }
  }

  // Library
  //
  while (this->Library_next ())
  {
    const ::std::string& r = this->Library ();

    if (this->Library_serializer_)
    {
      this->Library_serializer_->pre (r);
      this->_start_element ("Library");
      this->Library_serializer_->_pre_impl (ctx);

      if (ctx.error_type ())
        return;

      this->Library_serializer_->_serialize_attributes ();

      if (ctx.error_type ())
        return;

      this->Library_serializer_->_serialize_content ();

      if (ctx.error_type ())
        return;

      this->Library_serializer_->_post_impl ();

      if (ctx.error_type ())
        return;

      this->_end_element ();
      this->Library_serializer_->post ();
    }
  }
}

// Element validation and serialization for Files_sskel.
//
void Files_sskel::
_serialize_content ()
{
  ::xsde::cxx::serializer::context& ctx = this->_context ();

  // Include
  //
  while (this->Include_next ())
  {
    const ::std::string& r = this->Include ();

    if (this->Include_serializer_)
    {
      this->Include_serializer_->pre (r);
      this->_start_element ("Include");
      this->Include_serializer_->_pre_impl (ctx);

      if (ctx.error_type ())
        return;

      this->Include_serializer_->_serialize_attributes ();

      if (ctx.error_type ())
        return;

      this->Include_serializer_->_serialize_content ();

      if (ctx.error_type ())
        return;

      this->Include_serializer_->_post_impl ();

      if (ctx.error_type ())
        return;

      this->_end_element ();
      this->Include_serializer_->post ();
    }
  }

  // Library
  //
  while (this->Library_next ())
  {
    const ::std::string& r = this->Library ();

    if (this->Library_serializer_)
    {
      this->Library_serializer_->pre (r);
      this->_start_element ("Library");
      this->Library_serializer_->_pre_impl (ctx);

      if (ctx.error_type ())
        return;

      this->Library_serializer_->_serialize_attributes ();

      if (ctx.error_type ())
        return;

      this->Library_serializer_->_serialize_content ();

      if (ctx.error_type ())
        return;

      this->Library_serializer_->_post_impl ();

      if (ctx.error_type ())
        return;

      this->_end_element ();
      this->Library_serializer_->post ();
    }
  }
}

// Element validation and serialization for Directories1_sskel.
//
void Directories1_sskel::
_serialize_content ()
{
  ::xsde::cxx::serializer::context& ctx = this->_context ();

  // Include
  //
  while (this->Include_next ())
  {
    const ::std::string& r = this->Include ();

    if (this->Include_serializer_)
    {
      this->Include_serializer_->pre (r);
      this->_start_element ("Include");
      this->Include_serializer_->_pre_impl (ctx);

      if (ctx.error_type ())
        return;

      this->Include_serializer_->_serialize_attributes ();

      if (ctx.error_type ())
        return;

      this->Include_serializer_->_serialize_content ();

      if (ctx.error_type ())
        return;

      this->Include_serializer_->_post_impl ();

      if (ctx.error_type ())
        return;

      this->_end_element ();
      this->Include_serializer_->post ();
    }
  }

  // Library
  //
  while (this->Library_next ())
  {
    const ::std::string& r = this->Library ();

    if (this->Library_serializer_)
    {
      this->Library_serializer_->pre (r);
      this->_start_element ("Library");
      this->Library_serializer_->_pre_impl (ctx);

      if (ctx.error_type ())
        return;

      this->Library_serializer_->_serialize_attributes ();

      if (ctx.error_type ())
        return;

      this->Library_serializer_->_serialize_content ();

      if (ctx.error_type ())
        return;

      this->Library_serializer_->_post_impl ();

      if (ctx.error_type ())
        return;

      this->_end_element ();
      this->Library_serializer_->post ();
    }
  }
}

// Element validation and serialization for Files1_sskel.
//
void Files1_sskel::
_serialize_content ()
{
  ::xsde::cxx::serializer::context& ctx = this->_context ();

  // Include
  //
  while (this->Include_next ())
  {
    const ::std::string& r = this->Include ();

    if (this->Include_serializer_)
    {
      this->Include_serializer_->pre (r);
      this->_start_element ("Include");
      this->Include_serializer_->_pre_impl (ctx);

      if (ctx.error_type ())
        return;

      this->Include_serializer_->_serialize_attributes ();

      if (ctx.error_type ())
        return;

      this->Include_serializer_->_serialize_content ();

      if (ctx.error_type ())
        return;

      this->Include_serializer_->_post_impl ();

      if (ctx.error_type ())
        return;

      this->_end_element ();
      this->Include_serializer_->post ();
    }
  }

  // Library
  //
  while (this->Library_next ())
  {
    const ::std::string& r = this->Library ();

    if (this->Library_serializer_)
    {
      this->Library_serializer_->pre (r);
      this->_start_element ("Library");
      this->Library_serializer_->_pre_impl (ctx);

      if (ctx.error_type ())
        return;

      this->Library_serializer_->_serialize_attributes ();

      if (ctx.error_type ())
        return;

      this->Library_serializer_->_serialize_content ();

      if (ctx.error_type ())
        return;

      this->Library_serializer_->_post_impl ();

      if (ctx.error_type ())
        return;

      this->_end_element ();
      this->Library_serializer_->post ();
    }
  }
}

// Begin epilogue.
//
//
// End epilogue.

