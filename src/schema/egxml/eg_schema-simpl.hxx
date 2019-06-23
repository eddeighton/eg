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

#ifndef W__WORKSPACE_EG_SRC_SCHEMA_EG_SCHEMA_SIMPL_HXX
#define W__WORKSPACE_EG_SRC_SCHEMA_EG_SCHEMA_SIMPL_HXX

#include <xsde/cxx/pre.hxx>

// Begin prologue.
//
//
// End prologue.

#ifndef XSDE_OMIT_SAGGR
#  define XSDE_OMIT_SAGGR
#  define W__WORKSPACE_EG_SRC_SCHEMA_EG_SCHEMA_SIMPL_HXX_CLEAR_OMIT_SAGGR
#endif

#include "eg_schema-sskel.hxx"

#include <xsde/cxx/stack.hxx>

namespace egxml
{
  class Package_simpl: public Package_sskel
  {
    public:
    virtual void
    pre (const ::egxml::Package&);

    // Elements.
    //
    virtual ::std::string
    Name ();

    virtual bool
    Repository_present ();

    virtual ::std::string
    Repository ();

    virtual ::std::string
    License ();

    virtual bool
    Description_present ();

    virtual ::std::string
    Description ();

    virtual bool
    Directories_present ();

    virtual const ::egxml::Directories&
    Directories ();

    virtual bool
    Files_present ();

    virtual const ::egxml::Files&
    Files ();

    virtual bool
    Command_next ();

    virtual ::std::string
    Command ();

    public:
    struct Package_simpl_state
    {
      const ::egxml::Package* Package_;
      ::egxml::Package::Command_const_iterator Command_;
      ::egxml::Package::Command_const_iterator Command_end_;
    };

    Package_simpl_state Package_simpl_state_;
  };

  class Host_simpl: public Host_sskel
  {
    public:
    Host_simpl ();

    virtual void
    pre (const ::egxml::Host&);

    public:
    ::egxml::Package_simpl base_impl_;

    public:
    struct Host_simpl_state
    {
      const ::egxml::Host* Host_;
    };

    Host_simpl_state Host_simpl_state_;
  };

  class Build_simpl: public Build_sskel
  {
    public:
    virtual void
    pre (const ::egxml::Build&);

    // Elements.
    //
    virtual ::std::string
    Name ();

    virtual ::std::string
    CompilerFlags ();

    virtual ::std::string
    LinkerFlags ();

    public:
    struct Build_simpl_state
    {
      const ::egxml::Build* Build_;
    };

    Build_simpl_state Build_simpl_state_;
  };

  class Stack_simpl: public Stack_sskel
  {
    public:
    virtual void
    pre (const ::egxml::Stack&);

    // Elements.
    //
    virtual bool
    Size_present ();

    virtual unsigned int
    Size ();

    public:
    struct Stack_simpl_state
    {
      const ::egxml::Stack* Stack_;
    };

    Stack_simpl_state Stack_simpl_state_;
  };

  class Fibers_simpl: public Fibers_sskel
  {
    public:
    virtual void
    pre (const ::egxml::Fibers&);

    // Elements.
    //
    virtual bool
    Stack_present ();

    virtual const ::egxml::Stack&
    Stack ();

    public:
    struct Fibers_simpl_state
    {
      const ::egxml::Fibers* Fibers_;
    };

    Fibers_simpl_state Fibers_simpl_state_;
  };

  class Defaults_simpl: public Defaults_sskel
  {
    public:
    virtual void
    pre (const ::egxml::Defaults&);

    // Elements.
    //
    virtual bool
    Fibers_present ();

    virtual const ::egxml::Fibers&
    Fibers ();

    public:
    struct Defaults_simpl_state
    {
      const ::egxml::Defaults* Defaults_;
    };

    Defaults_simpl_state Defaults_simpl_state_;
  };

  class Project_simpl: public Project_sskel
  {
    public:
    virtual void
    pre (const ::egxml::Project&);

    // Elements.
    //
    virtual ::std::string
    Name ();

    virtual ::std::string
    Host ();

    virtual bool
    Description_present ();

    virtual ::std::string
    Description ();

    virtual bool
    Package_next ();

    virtual ::std::string
    Package ();

    virtual bool
    Build_next ();

    virtual const ::egxml::Build&
    Build ();

    virtual bool
    Run_next ();

    virtual const ::egxml::Run&
    Run ();

    virtual bool
    Defaults_present ();

    virtual const ::egxml::Defaults&
    Defaults ();

    public:
    struct Project_simpl_state
    {
      const ::egxml::Project* Project_;
      ::egxml::Project::Package_const_iterator Package_;
      ::egxml::Project::Package_const_iterator Package_end_;
      ::egxml::Project::Build_const_iterator Build_;
      ::egxml::Project::Build_const_iterator Build_end_;
      ::egxml::Project::Run_const_iterator Run_;
      ::egxml::Project::Run_const_iterator Run_end_;
    };

    Project_simpl_state Project_simpl_state_;
  };

  class EG_simpl: public EG_sskel
  {
    public:
    virtual void
    pre (const ::egxml::EG&);

    // Elements.
    //
    virtual choice_arm_tag
    choice_arm ();

    virtual const ::egxml::Package&
    Package ();

    virtual const ::egxml::Host&
    Host ();

    virtual const ::egxml::Project&
    Project ();

    public:
    struct EG_simpl_state
    {
      const ::egxml::EG* EG_;
    };

    EG_simpl_state EG_simpl_state_;
  };

  class Directories_simpl: public Directories_sskel
  {
    public:
    virtual void
    pre (const ::egxml::Directories&);

    // Elements.
    //
    virtual bool
    Include_next ();

    virtual ::std::string
    Include ();

    virtual bool
    Library_next ();

    virtual ::std::string
    Library ();

    public:
    struct Directories_simpl_state
    {
      const ::egxml::Directories* Directories_;
      ::egxml::Directories::Include_const_iterator Include_;
      ::egxml::Directories::Include_const_iterator Include_end_;
      ::egxml::Directories::Library_const_iterator Library_;
      ::egxml::Directories::Library_const_iterator Library_end_;
    };

    Directories_simpl_state Directories_simpl_state_;
  };

  class Files_simpl: public Files_sskel
  {
    public:
    virtual void
    pre (const ::egxml::Files&);

    // Elements.
    //
    virtual bool
    System_next ();

    virtual ::std::string
    System ();

    virtual bool
    Include_next ();

    virtual ::std::string
    Include ();

    virtual bool
    Source_next ();

    virtual ::std::string
    Source ();

    virtual bool
    Library_next ();

    virtual ::std::string
    Library ();

    public:
    struct Files_simpl_state
    {
      const ::egxml::Files* Files_;
      ::egxml::Files::System_const_iterator System_;
      ::egxml::Files::System_const_iterator System_end_;
      ::egxml::Files::Include_const_iterator Include_;
      ::egxml::Files::Include_const_iterator Include_end_;
      ::egxml::Files::Source_const_iterator Source_;
      ::egxml::Files::Source_const_iterator Source_end_;
      ::egxml::Files::Library_const_iterator Library_;
      ::egxml::Files::Library_const_iterator Library_end_;
    };

    Files_simpl_state Files_simpl_state_;
  };

  class Run_simpl: public Run_sskel
  {
    public:
    virtual void
    pre (const ::egxml::Run&);

    // Elements.
    //
    virtual ::std::string
    Name ();

    virtual ::std::string
    Command ();

    virtual bool
    Argument_next ();

    virtual ::std::string
    Argument ();

    public:
    struct Run_simpl_state
    {
      const ::egxml::Run* Run_;
      ::egxml::Run::Argument_const_iterator Argument_;
      ::egxml::Run::Argument_const_iterator Argument_end_;
    };

    Run_simpl_state Run_simpl_state_;
  };
}

#ifdef W__WORKSPACE_EG_SRC_SCHEMA_EG_SCHEMA_SIMPL_HXX_CLEAR_OMIT_SAGGR
#  undef XSDE_OMIT_SAGGR
#endif

#ifndef XSDE_OMIT_SAGGR

namespace egxml
{
  // Serializer aggregate for the EG element.
  //
  class EG_saggr
  {
    public:
    EG_saggr ();

    void
    pre (const ::egxml::EG& x)
    {
      this->EG_s_.pre (x);
    }

    void
    post ()
    {
      this->EG_s_.post ();
    }

    ::egxml::EG_simpl&
    root_serializer ()
    {
      return this->EG_s_;
    }

    static const char*
    root_name ();

    static const char*
    root_namespace ();

    void
    reset ()
    {
      this->EG_s_._reset ();
    }

    public:
    ::egxml::Files_simpl Files_s_;
    ::egxml::Host_simpl Host_s_;
    ::egxml::Project_simpl Project_s_;
    ::egxml::Build_simpl Build_s_;
    ::egxml::Run_simpl Run_s_;
    ::egxml::Defaults_simpl Defaults_s_;
    ::egxml::EG_simpl EG_s_;
    ::egxml::Package_simpl Package_s_;
    ::egxml::Directories_simpl Directories_s_;
    ::egxml::Fibers_simpl Fibers_s_;
    ::egxml::Stack_simpl Stack_s_;
    ::xml_schema::unsigned_int_simpl unsigned_int_s_;
    ::xml_schema::string_simpl string_s_;
  };
}

#endif // XSDE_OMIT_SAGGR

// Begin epilogue.
//
//
// End epilogue.

#include <xsde/cxx/post.hxx>

#endif // W__WORKSPACE_EG_SRC_SCHEMA_EG_SCHEMA_SIMPL_HXX