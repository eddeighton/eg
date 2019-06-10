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

#include "schema-pimpl.hxx"

#include <xsde/cxx/parser/validating/string-common.hxx>

namespace test
{
  // name_pimpl
  //

  name_pimpl::
  name_pimpl ()
  : name_pskel (&base_impl_)
  {
  }

  void name_pimpl::
  pre ()
  {
    this->base_impl_.pre ();
    this->name_pimpl_state_.name_ = ::test::name ();
  }

  ::test::name name_pimpl::
  post_name ()
  {
    static_cast< ::std::string& > (this->name_pimpl_state_.name_) = 
    this->base_impl_.post_string ();
    return this->name_pimpl_state_.name_;
  }

  // Config_pimpl
  //

  void Config_pimpl::
  pre ()
  {
    this->Config_pimpl_state_.Config_ = ::test::Config ();
  }

  void Config_pimpl::
  name (const ::test::name& x)
  {
    this->Config_pimpl_state_.Config_.name (x);
  }

  void Config_pimpl::
  file (bool x)
  {
    this->Config_pimpl_state_.Config_.file (x);
  }

  ::test::Config Config_pimpl::
  post_Config ()
  {
    return this->Config_pimpl_state_.Config_;
  }

  // eg_pimpl
  //

  eg_pimpl::
  eg_pimpl (bool b)
  {
    this->eg_pimpl_base_ = b;
    this->eg_pimpl_state_.eg_ = 0;
  }

  eg_pimpl::
  ~eg_pimpl ()
  {
    if (!this->eg_pimpl_base_ && this->eg_pimpl_state_.eg_)
      delete this->eg_pimpl_state_.eg_;
  }

  void eg_pimpl::
  _reset ()
  {
    eg_pskel::_reset ();

    if (!this->eg_pimpl_base_ && this->eg_pimpl_state_.eg_)
    {
      delete this->eg_pimpl_state_.eg_;
      this->eg_pimpl_state_.eg_ = 0;
    }
  }

  void eg_pimpl::
  pre_impl (::test::eg* x)
  {
    this->eg_pimpl_state_.eg_ = x;
  }

  void eg_pimpl::
  pre ()
  {
    ::test::eg* x = new ::test::eg;
    this->pre_impl (x);
  }

  void eg_pimpl::
  host (const ::std::string& x)
  {
    this->eg_pimpl_state_.eg_->host (x);
  }

  void eg_pimpl::
  folder (const ::std::string& x)
  {
    this->eg_pimpl_state_.eg_->folder (x);
  }

  void eg_pimpl::
  stuff (const ::test::Config& x)
  {
    this->eg_pimpl_state_.eg_->stuff ().push_back (x);
  }

  ::test::eg* eg_pimpl::
  post_eg ()
  {
    ::test::eg* r = this->eg_pimpl_state_.eg_;
    this->eg_pimpl_state_.eg_ = 0;
    return r;
  }
}

namespace test
{
  // eg_paggr
  //

  eg_paggr::
  eg_paggr ()
  {
    this->eg_p_.parsers (this->string_p_,
                         this->string_p_,
                         this->Config_p_);

    this->Config_p_.parsers (this->name_p_,
                             this->boolean_p_);
  }

  const char* eg_paggr::
  root_name ()
  {
    return "eg";
  }

  const char* eg_paggr::
  root_namespace ()
  {
    return "";
  }
}

// Begin epilogue.
//
//
// End epilogue.

