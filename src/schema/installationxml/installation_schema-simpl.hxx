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

#ifndef W__WORKSPACE_EG_SRC_SCHEMA_INSTALLATION_SCHEMA_SIMPL_HXX
#define W__WORKSPACE_EG_SRC_SCHEMA_INSTALLATION_SCHEMA_SIMPL_HXX

#include <xsde/cxx/pre.hxx>

// Begin prologue.
//
//
// End prologue.

#ifndef XSDE_OMIT_SAGGR
#  define XSDE_OMIT_SAGGR
#  define W__WORKSPACE_EG_SRC_SCHEMA_INSTALLATION_SCHEMA_SIMPL_HXX_CLEAR_OMIT_SAGGR
#endif

#include "installation_schema-sskel.hxx"

#include <xsde/cxx/stack.hxx>

namespace installationxml
{
  class Installation_simpl: public Installation_sskel
  {
    public:
    virtual void
    pre (const ::installationxml::Installation&);

    // Elements.
    //
    virtual ::std::string
    Version ();

    virtual ::std::string
    Python ();

    virtual ::std::string
    Toolchain ();

    virtual ::std::string
    SDK ();

    public:
    struct Installation_simpl_state
    {
      const ::installationxml::Installation* Installation_;
    };

    Installation_simpl_state Installation_simpl_state_;
  };
}

#ifdef W__WORKSPACE_EG_SRC_SCHEMA_INSTALLATION_SCHEMA_SIMPL_HXX_CLEAR_OMIT_SAGGR
#  undef XSDE_OMIT_SAGGR
#endif

#ifndef XSDE_OMIT_SAGGR

namespace installationxml
{
  // Serializer aggregate for the Installation element.
  //
  class Installation_saggr
  {
    public:
    Installation_saggr ();

    void
    pre (const ::installationxml::Installation& x)
    {
      this->Installation_s_.pre (x);
    }

    void
    post ()
    {
      this->Installation_s_.post ();
    }

    ::installationxml::Installation_simpl&
    root_serializer ()
    {
      return this->Installation_s_;
    }

    static const char*
    root_name ();

    static const char*
    root_namespace ();

    void
    reset ()
    {
      this->Installation_s_._reset ();
    }

    public:
    ::installationxml::Installation_simpl Installation_s_;
    ::xml_schema::string_simpl string_s_;
  };
}

#endif // XSDE_OMIT_SAGGR

// Begin epilogue.
//
//
// End epilogue.

#include <xsde/cxx/post.hxx>

#endif // W__WORKSPACE_EG_SRC_SCHEMA_INSTALLATION_SCHEMA_SIMPL_HXX