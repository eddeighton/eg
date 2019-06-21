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

#ifndef W__WORKSPACE_EG_SRC_SCHEMA_WIZARD_SCHEMA_SIMPL_HXX
#define W__WORKSPACE_EG_SRC_SCHEMA_WIZARD_SCHEMA_SIMPL_HXX

#include <xsde/cxx/pre.hxx>

// Begin prologue.
//
//
// End prologue.

#ifndef XSDE_OMIT_SAGGR
#  define XSDE_OMIT_SAGGR
#  define W__WORKSPACE_EG_SRC_SCHEMA_WIZARD_SCHEMA_SIMPL_HXX_CLEAR_OMIT_SAGGR
#endif

#include "wizard_schema-sskel.hxx"

#include <xsde/cxx/stack.hxx>

namespace wizardxml
{
  class Wizard_simpl: public Wizard_sskel
  {
    public:
    virtual void
    pre (const ::wizardxml::Wizard&);

    // Elements.
    //
    virtual bool
    Project_next ();

    virtual ::std::string
    Project ();

    public:
    struct Wizard_simpl_state
    {
      const ::wizardxml::Wizard* Wizard_;
      ::wizardxml::Wizard::Project_const_iterator Project_;
      ::wizardxml::Wizard::Project_const_iterator Project_end_;
    };

    Wizard_simpl_state Wizard_simpl_state_;
  };
}

#ifdef W__WORKSPACE_EG_SRC_SCHEMA_WIZARD_SCHEMA_SIMPL_HXX_CLEAR_OMIT_SAGGR
#  undef XSDE_OMIT_SAGGR
#endif

#ifndef XSDE_OMIT_SAGGR

namespace wizardxml
{
  // Serializer aggregate for the Wizard element.
  //
  class Wizard_saggr
  {
    public:
    Wizard_saggr ();

    void
    pre (const ::wizardxml::Wizard& x)
    {
      this->Wizard_s_.pre (x);
    }

    void
    post ()
    {
      this->Wizard_s_.post ();
    }

    ::wizardxml::Wizard_simpl&
    root_serializer ()
    {
      return this->Wizard_s_;
    }

    static const char*
    root_name ();

    static const char*
    root_namespace ();

    void
    reset ()
    {
      this->Wizard_s_._reset ();
    }

    public:
    ::xml_schema::string_simpl string_s_;
    ::wizardxml::Wizard_simpl Wizard_s_;
  };
}

#endif // XSDE_OMIT_SAGGR

// Begin epilogue.
//
//
// End epilogue.

#include <xsde/cxx/post.hxx>

#endif // W__WORKSPACE_EG_SRC_SCHEMA_WIZARD_SCHEMA_SIMPL_HXX
