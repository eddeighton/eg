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

#include <xsde/cxx/pre.hxx>

// Begin prologue.
//
//
// End prologue.

#include "installation_schema.hxx"

namespace installationxml
{
  // Installation
  //

  const ::std::string& Installation::
  Version () const
  {
    return this->Version_;
  }

  ::std::string& Installation::
  Version ()
  {
    return this->Version_;
  }

  void Installation::
  Version (const ::std::string& x)
  {
    this->Version_ = x;
  }

  const ::std::string& Installation::
  Python () const
  {
    return this->Python_;
  }

  ::std::string& Installation::
  Python ()
  {
    return this->Python_;
  }

  void Installation::
  Python (const ::std::string& x)
  {
    this->Python_ = x;
  }

  const ::std::string& Installation::
  Toolchain () const
  {
    return this->Toolchain_;
  }

  ::std::string& Installation::
  Toolchain ()
  {
    return this->Toolchain_;
  }

  void Installation::
  Toolchain (const ::std::string& x)
  {
    this->Toolchain_ = x;
  }

  const ::std::string& Installation::
  SDK () const
  {
    return this->SDK_;
  }

  ::std::string& Installation::
  SDK ()
  {
    return this->SDK_;
  }

  void Installation::
  SDK (const ::std::string& x)
  {
    this->SDK_ = x;
  }
}

#include <stdlib.h>
#include <new>

namespace installationxml
{
  // Installation
  //

  Installation::
  Installation ()
  {
  }

  Installation::
  ~Installation ()
  {
  }

  Installation::
  Installation (const Installation& x)
  {
    XSDE_UNUSED (x);
    this->Version_ = x.Version_;
    this->Python_ = x.Python_;
    this->Toolchain_ = x.Toolchain_;
    this->SDK_ = x.SDK_;
  }

  Installation& Installation::
  operator= (const Installation& x)
  {
    XSDE_UNUSED (x);
    this->Version_ = x.Version_;
    this->Python_ = x.Python_;
    this->Toolchain_ = x.Toolchain_;
    this->SDK_ = x.SDK_;
    return *this;
  }
}

// Begin epilogue.
//
//
// End epilogue.

#include <xsde/cxx/post.hxx>
