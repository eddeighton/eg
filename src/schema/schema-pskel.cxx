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

#include "schema-pskel.hxx"

// name_pskel
//

name_pskel::
name_pskel (::xml_schema::string_pskel* tiein)
: ::xml_schema::string_pskel (tiein, 0),
  name_impl_ (0)
{
  this->_min_length_facet (1UL);
}

name_pskel::
name_pskel (name_pskel* impl, void*)
: ::xml_schema::string_pskel (impl, 0),
  name_impl_ (impl)
{
  this->_min_length_facet (1UL);
}

// Config_pskel
//

void Config_pskel::
name_parser (::name_pskel& p)
{
  this->name_parser_ = &p;
}

void Config_pskel::
file_parser (::xml_schema::boolean_pskel& p)
{
  this->file_parser_ = &p;
}

void Config_pskel::
parsers (::name_pskel& name,
         ::xml_schema::boolean_pskel& file)
{
  this->name_parser_ = &name;
  this->file_parser_ = &file;
}

Config_pskel::
Config_pskel ()
: Config_impl_ (0),
  name_parser_ (0),
  file_parser_ (0),
  v_state_stack_ (sizeof (v_state_), &v_state_first_)
{
}

Config_pskel::
Config_pskel (Config_pskel* impl, void*)
: ::xsde::cxx::parser::validating::complex_content (impl, 0),
  Config_impl_ (impl),
  name_parser_ (0),
  file_parser_ (0),
  v_state_stack_ (sizeof (v_state_), &v_state_first_)
{
}

// eg_pskel
//

void eg_pskel::
host_parser (::xml_schema::string_pskel& p)
{
  this->host_parser_ = &p;
}

void eg_pskel::
folder_parser (::xml_schema::string_pskel& p)
{
  this->folder_parser_ = &p;
}

void eg_pskel::
stuff_parser (::Config_pskel& p)
{
  this->stuff_parser_ = &p;
}

void eg_pskel::
parsers (::xml_schema::string_pskel& host,
         ::xml_schema::string_pskel& folder,
         ::Config_pskel& stuff)
{
  this->host_parser_ = &host;
  this->folder_parser_ = &folder;
  this->stuff_parser_ = &stuff;
}

eg_pskel::
eg_pskel ()
: eg_impl_ (0),
  host_parser_ (0),
  folder_parser_ (0),
  stuff_parser_ (0),
  v_state_stack_ (sizeof (v_state_), &v_state_first_)
{
}

eg_pskel::
eg_pskel (eg_pskel* impl, void*)
: ::xsde::cxx::parser::validating::complex_content (impl, 0),
  eg_impl_ (impl),
  host_parser_ (0),
  folder_parser_ (0),
  stuff_parser_ (0),
  v_state_stack_ (sizeof (v_state_), &v_state_first_)
{
}

#include <assert.h>

// name_pskel
//

::std::string name_pskel::
post_string ()
{
  assert (this->string_impl_);
  return this->string_impl_->post_string ();
}

// Config_pskel
//

void Config_pskel::
name (const ::name& x)
{
  if (this->Config_impl_)
    this->Config_impl_->name (x);
}

void Config_pskel::
file (bool x)
{
  if (this->Config_impl_)
    this->Config_impl_->file (x);
}

void Config_pskel::
_reset ()
{
  if (this->resetting_)
    return;

  typedef ::xsde::cxx::parser::validating::complex_content base;
  base::_reset ();

  this->v_state_stack_.clear ();

  this->resetting_ = true;

  if (this->name_parser_)
    this->name_parser_->_reset ();

  if (this->file_parser_)
    this->file_parser_->_reset ();

  this->resetting_ = false;
}

// eg_pskel
//

void eg_pskel::
host (const ::std::string& x)
{
  if (this->eg_impl_)
    this->eg_impl_->host (x);
}

void eg_pskel::
folder (const ::std::string& x)
{
  if (this->eg_impl_)
    this->eg_impl_->folder (x);
}

void eg_pskel::
stuff (const ::Config& x)
{
  if (this->eg_impl_)
    this->eg_impl_->stuff (x);
}

void eg_pskel::
_reset ()
{
  if (this->resetting_)
    return;

  typedef ::xsde::cxx::parser::validating::complex_content base;
  base::_reset ();

  this->v_state_stack_.clear ();

  this->resetting_ = true;

  if (this->host_parser_)
    this->host_parser_->_reset ();

  if (this->folder_parser_)
    this->folder_parser_->_reset ();

  if (this->stuff_parser_)
    this->stuff_parser_->_reset ();

  this->resetting_ = false;
}

#include <assert.h>

// Element validation and dispatch functions for Config_pskel.
//
bool Config_pskel::
_start_element_impl (const ::xsde::cxx::ro_string& ns,
                     const ::xsde::cxx::ro_string& n)
{
  ::xsde::cxx::parser::context& ctx = this->_context ();

  v_state_& vs = *static_cast< v_state_* > (this->v_state_stack_.top ());
  v_state_descr_* vd = vs.data + (vs.size - 1);

  if (vd->func == 0 && vd->state == 0)
  {
    typedef ::xsde::cxx::parser::validating::complex_content base;
    if (base::_start_element_impl (ns, n))
      return true;
    else
      vd->state = 1;
  }

  while (vd->func != 0)
  {
    (this->*vd->func) (vd->state, vd->count, ns, n, true);

    vd = vs.data + (vs.size - 1);

    if (vd->state == ~0UL && !ctx.error_type ())
      vd = vs.data + (--vs.size - 1);
    else
      break;
  }

  if (vd->func == 0)
  {
    if (vd->state != ~0UL)
    {
      unsigned long s = ~0UL;

      if (n == "name" && ns.empty ())
        s = 0UL;

      if (s != ~0UL)
      {
        vd->count++;
        vd->state = ~0UL;

        vd = vs.data + vs.size++;
        vd->func = &Config_pskel::sequence_0;
        vd->state = s;
        vd->count = 0;

        this->sequence_0 (vd->state, vd->count, ns, n, true);
      }
      else
      {
        if (vd->count < 1UL)
        {
          this->_schema_error (::xsde::cxx::schema_error::expected_element);
          return true;
        }

        return false;
      }
    }
    else
      return false;
  }

  return true;
}

bool Config_pskel::
_end_element_impl (const ::xsde::cxx::ro_string& ns,
                   const ::xsde::cxx::ro_string& n)
{
  v_state_& vs = *static_cast< v_state_* > (this->v_state_stack_.top ());
  v_state_descr_& vd = vs.data[vs.size - 1];

  if (vd.func == 0 && vd.state == 0)
  {
    typedef ::xsde::cxx::parser::validating::complex_content base;
    if (!base::_end_element_impl (ns, n))
      assert (false);
    return true;
  }

  assert (vd.func != 0);
  (this->*vd.func) (vd.state, vd.count, ns, n, false);

  if (vd.state == ~0UL)
    vs.size--;

  return true;
}

void Config_pskel::
_pre_e_validate ()
{
  this->v_state_stack_.push ();
  static_cast< v_state_* > (this->v_state_stack_.top ())->size = 0;

  v_state_& vs = *static_cast< v_state_* > (this->v_state_stack_.top ());
  v_state_descr_& vd = vs.data[vs.size++];

  vd.func = 0;
  vd.state = 0;
  vd.count = 0;
}

void Config_pskel::
_post_e_validate ()
{
  ::xsde::cxx::parser::context& ctx = this->_context ();

  v_state_& vs = *static_cast< v_state_* > (this->v_state_stack_.top ());
  v_state_descr_* vd = vs.data + (vs.size - 1);

  ::xsde::cxx::ro_string empty;
  while (vd->func != 0)
  {
    (this->*vd->func) (vd->state, vd->count, empty, empty, true);

    if (ctx.error_type ())
      return;

    assert (vd->state == ~0UL);
    vd = vs.data + (--vs.size - 1);
  }

  if (vd->count < 1UL)
    this->_schema_error (::xsde::cxx::schema_error::expected_element);

  this->v_state_stack_.pop ();
}

void Config_pskel::
sequence_0 (unsigned long& state,
            unsigned long& count,
            const ::xsde::cxx::ro_string& ns,
            const ::xsde::cxx::ro_string& n,
            bool start)
{
  ::xsde::cxx::parser::context& ctx = this->_context ();

  XSDE_UNUSED (ctx);

  switch (state)
  {
    case 0UL:
    {
      if (n == "name" && ns.empty ())
      {
        if (start)
        {
          if (this->name_parser_)
          {
            this->name_parser_->pre ();
            ctx.nested_parser (this->name_parser_);
          }
        }
        else
        {
          if (this->name_parser_ != 0)
          {
            const ::name& tmp = this->name_parser_->post_name ();
            this->name (tmp);
          }

          count = 0;
          state = 1UL;
        }

        break;
      }
      else
      {
        assert (start);
        if (count < 1UL)
        {
          this->_schema_error (::xsde::cxx::schema_error::expected_element);
          break;
        }

        count = 0;
        state = 1UL;
        // Fall through.
      }
    }
    case 1UL:
    {
      if (n == "file" && ns.empty ())
      {
        if (start)
        {
          if (this->file_parser_)
          {
            this->file_parser_->pre ();
            ctx.nested_parser (this->file_parser_);
          }
        }
        else
        {
          if (this->file_parser_ != 0)
          {
            bool tmp = this->file_parser_->post_boolean ();
            this->file (tmp);
          }

          count = 0;
          state = ~0UL;
        }

        break;
      }
      else
      {
        assert (start);
        if (count < 1UL)
        {
          this->_schema_error (::xsde::cxx::schema_error::expected_element);
          break;
        }

        count = 0;
        state = ~0UL;
        // Fall through.
      }
    }
    case ~0UL:
      break;
  }
}

// Element validation and dispatch functions for eg_pskel.
//
bool eg_pskel::
_start_element_impl (const ::xsde::cxx::ro_string& ns,
                     const ::xsde::cxx::ro_string& n)
{
  ::xsde::cxx::parser::context& ctx = this->_context ();

  v_state_& vs = *static_cast< v_state_* > (this->v_state_stack_.top ());
  v_state_descr_* vd = vs.data + (vs.size - 1);

  if (vd->func == 0 && vd->state == 0)
  {
    typedef ::xsde::cxx::parser::validating::complex_content base;
    if (base::_start_element_impl (ns, n))
      return true;
    else
      vd->state = 1;
  }

  while (vd->func != 0)
  {
    (this->*vd->func) (vd->state, vd->count, ns, n, true);

    vd = vs.data + (vs.size - 1);

    if (vd->state == ~0UL && !ctx.error_type ())
      vd = vs.data + (--vs.size - 1);
    else
      break;
  }

  if (vd->func == 0)
  {
    if (vd->state != ~0UL)
    {
      unsigned long s = ~0UL;

      if (n == "host" && ns.empty ())
        s = 0UL;

      if (s != ~0UL)
      {
        vd->count++;
        vd->state = ~0UL;

        vd = vs.data + vs.size++;
        vd->func = &eg_pskel::sequence_0;
        vd->state = s;
        vd->count = 0;

        this->sequence_0 (vd->state, vd->count, ns, n, true);
      }
      else
      {
        if (vd->count < 1UL)
        {
          this->_schema_error (::xsde::cxx::schema_error::expected_element);
          return true;
        }

        return false;
      }
    }
    else
      return false;
  }

  return true;
}

bool eg_pskel::
_end_element_impl (const ::xsde::cxx::ro_string& ns,
                   const ::xsde::cxx::ro_string& n)
{
  v_state_& vs = *static_cast< v_state_* > (this->v_state_stack_.top ());
  v_state_descr_& vd = vs.data[vs.size - 1];

  if (vd.func == 0 && vd.state == 0)
  {
    typedef ::xsde::cxx::parser::validating::complex_content base;
    if (!base::_end_element_impl (ns, n))
      assert (false);
    return true;
  }

  assert (vd.func != 0);
  (this->*vd.func) (vd.state, vd.count, ns, n, false);

  if (vd.state == ~0UL)
    vs.size--;

  return true;
}

void eg_pskel::
_pre_e_validate ()
{
  this->v_state_stack_.push ();
  static_cast< v_state_* > (this->v_state_stack_.top ())->size = 0;

  v_state_& vs = *static_cast< v_state_* > (this->v_state_stack_.top ());
  v_state_descr_& vd = vs.data[vs.size++];

  vd.func = 0;
  vd.state = 0;
  vd.count = 0;
}

void eg_pskel::
_post_e_validate ()
{
  ::xsde::cxx::parser::context& ctx = this->_context ();

  v_state_& vs = *static_cast< v_state_* > (this->v_state_stack_.top ());
  v_state_descr_* vd = vs.data + (vs.size - 1);

  ::xsde::cxx::ro_string empty;
  while (vd->func != 0)
  {
    (this->*vd->func) (vd->state, vd->count, empty, empty, true);

    if (ctx.error_type ())
      return;

    assert (vd->state == ~0UL);
    vd = vs.data + (--vs.size - 1);
  }

  if (vd->count < 1UL)
    this->_schema_error (::xsde::cxx::schema_error::expected_element);

  this->v_state_stack_.pop ();
}

void eg_pskel::
sequence_0 (unsigned long& state,
            unsigned long& count,
            const ::xsde::cxx::ro_string& ns,
            const ::xsde::cxx::ro_string& n,
            bool start)
{
  ::xsde::cxx::parser::context& ctx = this->_context ();

  XSDE_UNUSED (ctx);

  switch (state)
  {
    case 0UL:
    {
      if (n == "host" && ns.empty ())
      {
        if (start)
        {
          if (this->host_parser_)
          {
            this->host_parser_->pre ();
            ctx.nested_parser (this->host_parser_);
          }
        }
        else
        {
          if (this->host_parser_ != 0)
          {
            const ::std::string& tmp = this->host_parser_->post_string ();
            this->host (tmp);
          }

          count = 0;
          state = 1UL;
        }

        break;
      }
      else
      {
        assert (start);
        if (count < 1UL)
        {
          this->_schema_error (::xsde::cxx::schema_error::expected_element);
          break;
        }

        count = 0;
        state = 1UL;
        // Fall through.
      }
    }
    case 1UL:
    {
      if (n == "folder" && ns.empty ())
      {
        if (start)
        {
          if (this->folder_parser_)
          {
            this->folder_parser_->pre ();
            ctx.nested_parser (this->folder_parser_);
          }
        }
        else
        {
          if (this->folder_parser_ != 0)
          {
            const ::std::string& tmp = this->folder_parser_->post_string ();
            this->folder (tmp);
          }

          count = 0;
          state = 2UL;
        }

        break;
      }
      else
      {
        assert (start);
        if (count < 1UL)
        {
          this->_schema_error (::xsde::cxx::schema_error::expected_element);
          break;
        }

        count = 0;
        state = 2UL;
        // Fall through.
      }
    }
    case 2UL:
    {
      if (n == "stuff" && ns.empty ())
      {
        if (start)
        {
          if (this->stuff_parser_)
          {
            this->stuff_parser_->pre ();
            ctx.nested_parser (this->stuff_parser_);
          }
        }
        else
        {
          if (this->stuff_parser_ != 0)
          {
            const ::Config& tmp = this->stuff_parser_->post_Config ();
            this->stuff (tmp);
          }

          count++;
        }

        break;
      }
      else
      {
        assert (start);
        count = 0;
        state = ~0UL;
        // Fall through.
      }
    }
    case ~0UL:
      break;
  }
}

// Begin epilogue.
//
//
// End epilogue.

