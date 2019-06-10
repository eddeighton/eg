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

#include "schema-simpl.hxx"

#include <xsde/cxx/serializer/validating/string-common.hxx>

namespace test
{
  // name_simpl
  //

  name_simpl::
  name_simpl ()
  : name_sskel (&base_impl_)
  {
  }

  void name_simpl::
  pre (const ::test::name& x)
  {
    this->base_impl_.pre (x);
  }

  // Config_simpl
  //

  void Config_simpl::
  pre (const ::test::Config& x)
  {
    this->Config_simpl_state_.Config_ = &x;
  }

  const ::test::name& Config_simpl::
  name ()
  {
    return this->Config_simpl_state_.Config_->name ();
  }

  bool Config_simpl::
  file ()
  {
    return this->Config_simpl_state_.Config_->file ();
  }

  // eg_simpl
  //

  void eg_simpl::
  pre (const ::test::eg& x)
  {
    this->eg_simpl_state_.eg_ = &x;
    this->eg_simpl_state_.stuff_ = 
    this->eg_simpl_state_.eg_->stuff ().begin ();
    this->eg_simpl_state_.stuff_end_ = 
    this->eg_simpl_state_.eg_->stuff ().end ();
  }

  ::std::string eg_simpl::
  host ()
  {
    return this->eg_simpl_state_.eg_->host ();
  }

  ::std::string eg_simpl::
  folder ()
  {
    return this->eg_simpl_state_.eg_->folder ();
  }

  bool eg_simpl::
  stuff_next ()
  {
    return this->eg_simpl_state_.stuff_ != 
    this->eg_simpl_state_.stuff_end_;
  }

  const ::test::Config& eg_simpl::
  stuff ()
  {
    return *this->eg_simpl_state_.stuff_++;
  }
}

namespace test
{
  // eg_saggr
  //

  eg_saggr::
  eg_saggr ()
  {
    this->eg_s_.serializers (this->string_s_,
                             this->string_s_,
                             this->Config_s_);

    this->Config_s_.serializers (this->name_s_,
                                 this->boolean_s_);
  }

  const char* eg_saggr::
  root_name ()
  {
    return "eg";
  }

  const char* eg_saggr::
  root_namespace ()
  {
    return "";
  }
}

// Begin epilogue.
//
//
// End epilogue.

