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

#include "eg_schema.hxx"

namespace egxml
{
  // Package
  //

  const ::std::string& Package::
  Name () const
  {
    return this->Name_;
  }

  ::std::string& Package::
  Name ()
  {
    return this->Name_;
  }

  void Package::
  Name (const ::std::string& x)
  {
    this->Name_ = x;
  }

  bool Package::
  Repository_present () const
  {
    return this->Repository_present_;
  }

  void Package::
  Repository_present (bool x)
  {
    this->Repository_present_ = x;
  }

  const ::std::string& Package::
  Repository () const
  {
    return this->Repository_;
  }

  ::std::string& Package::
  Repository ()
  {
    return this->Repository_;
  }

  void Package::
  Repository (const ::std::string& x)
  {
    this->Repository_ = x;
    this->Repository_present_ = true;
  }

  const ::std::string& Package::
  License () const
  {
    return this->License_;
  }

  ::std::string& Package::
  License ()
  {
    return this->License_;
  }

  void Package::
  License (const ::std::string& x)
  {
    this->License_ = x;
  }

  bool Package::
  Description_present () const
  {
    return this->Description_present_;
  }

  void Package::
  Description_present (bool x)
  {
    this->Description_present_ = x;
  }

  const ::std::string& Package::
  Description () const
  {
    return this->Description_;
  }

  ::std::string& Package::
  Description ()
  {
    return this->Description_;
  }

  void Package::
  Description (const ::std::string& x)
  {
    this->Description_ = x;
    this->Description_present_ = true;
  }

  bool Package::
  Directories_present () const
  {
    return this->Directories_ != 0;
  }

  const ::egxml::Directories& Package::
  Directories () const
  {
    return *this->Directories_;
  }

  ::egxml::Directories& Package::
  Directories ()
  {
    return *this->Directories_;
  }

  void Package::
  Directories (::egxml::Directories* x)
  {
    delete this->Directories_;
    this->Directories_ = x;
  }

  bool Package::
  Files_present () const
  {
    return this->Files_ != 0;
  }

  const ::egxml::Files& Package::
  Files () const
  {
    return *this->Files_;
  }

  ::egxml::Files& Package::
  Files ()
  {
    return *this->Files_;
  }

  void Package::
  Files (::egxml::Files* x)
  {
    delete this->Files_;
    this->Files_ = x;
  }

  const Package::Command_sequence& Package::
  Command () const
  {
    return this->Command_;
  }

  Package::Command_sequence& Package::
  Command ()
  {
    return this->Command_;
  }

  // Host
  //

  bool Host::
  CompilerFlags_present () const
  {
    return this->CompilerFlags_present_;
  }

  void Host::
  CompilerFlags_present (bool x)
  {
    this->CompilerFlags_present_ = x;
  }

  const ::std::string& Host::
  CompilerFlags () const
  {
    return this->CompilerFlags_;
  }

  ::std::string& Host::
  CompilerFlags ()
  {
    return this->CompilerFlags_;
  }

  void Host::
  CompilerFlags (const ::std::string& x)
  {
    this->CompilerFlags_ = x;
    this->CompilerFlags_present_ = true;
  }

  bool Host::
  LinkerFlags_present () const
  {
    return this->LinkerFlags_present_;
  }

  void Host::
  LinkerFlags_present (bool x)
  {
    this->LinkerFlags_present_ = x;
  }

  const ::std::string& Host::
  LinkerFlags () const
  {
    return this->LinkerFlags_;
  }

  ::std::string& Host::
  LinkerFlags ()
  {
    return this->LinkerFlags_;
  }

  void Host::
  LinkerFlags (const ::std::string& x)
  {
    this->LinkerFlags_ = x;
    this->LinkerFlags_present_ = true;
  }

  // Build
  //

  const ::std::string& Build::
  Name () const
  {
    return this->Name_;
  }

  ::std::string& Build::
  Name ()
  {
    return this->Name_;
  }

  void Build::
  Name (const ::std::string& x)
  {
    this->Name_ = x;
  }

  const ::std::string& Build::
  CompilerFlags () const
  {
    return this->CompilerFlags_;
  }

  ::std::string& Build::
  CompilerFlags ()
  {
    return this->CompilerFlags_;
  }

  void Build::
  CompilerFlags (const ::std::string& x)
  {
    this->CompilerFlags_ = x;
  }

  const ::std::string& Build::
  LinkerFlags () const
  {
    return this->LinkerFlags_;
  }

  ::std::string& Build::
  LinkerFlags ()
  {
    return this->LinkerFlags_;
  }

  void Build::
  LinkerFlags (const ::std::string& x)
  {
    this->LinkerFlags_ = x;
  }

  // Stack
  //

  bool Stack::
  Size_present () const
  {
    return this->Size_present_;
  }

  void Stack::
  Size_present (bool x)
  {
    this->Size_present_ = x;
  }

  unsigned int Stack::
  Size () const
  {
    return this->Size_;
  }

  unsigned int& Stack::
  Size ()
  {
    return this->Size_;
  }

  void Stack::
  Size (unsigned int x)
  {
    this->Size_ = x;
    this->Size_present_ = true;
  }

  // Fibers
  //

  bool Fibers::
  Stack_present () const
  {
    return this->Stack_present_;
  }

  void Fibers::
  Stack_present (bool x)
  {
    this->Stack_present_ = x;
  }

  const ::egxml::Stack& Fibers::
  Stack () const
  {
    return this->Stack_;
  }

  ::egxml::Stack& Fibers::
  Stack ()
  {
    return this->Stack_;
  }

  void Fibers::
  Stack (const ::egxml::Stack& x)
  {
    this->Stack_ = x;
    this->Stack_present_ = true;
  }

  // Defaults
  //

  bool Defaults::
  Fibers_present () const
  {
    return this->Fibers_present_;
  }

  void Defaults::
  Fibers_present (bool x)
  {
    this->Fibers_present_ = x;
  }

  const ::egxml::Fibers& Defaults::
  Fibers () const
  {
    return this->Fibers_;
  }

  ::egxml::Fibers& Defaults::
  Fibers ()
  {
    return this->Fibers_;
  }

  void Defaults::
  Fibers (const ::egxml::Fibers& x)
  {
    this->Fibers_ = x;
    this->Fibers_present_ = true;
  }

  // Project
  //

  const ::std::string& Project::
  Name () const
  {
    return this->Name_;
  }

  ::std::string& Project::
  Name ()
  {
    return this->Name_;
  }

  void Project::
  Name (const ::std::string& x)
  {
    this->Name_ = x;
  }

  const ::std::string& Project::
  Host () const
  {
    return this->Host_;
  }

  ::std::string& Project::
  Host ()
  {
    return this->Host_;
  }

  void Project::
  Host (const ::std::string& x)
  {
    this->Host_ = x;
  }

  bool Project::
  Description_present () const
  {
    return this->Description_present_;
  }

  void Project::
  Description_present (bool x)
  {
    this->Description_present_ = x;
  }

  const ::std::string& Project::
  Description () const
  {
    return this->Description_;
  }

  ::std::string& Project::
  Description ()
  {
    return this->Description_;
  }

  void Project::
  Description (const ::std::string& x)
  {
    this->Description_ = x;
    this->Description_present_ = true;
  }

  const Project::Package_sequence& Project::
  Package () const
  {
    return this->Package_;
  }

  Project::Package_sequence& Project::
  Package ()
  {
    return this->Package_;
  }

  bool Project::
  Files_present () const
  {
    return this->Files_ != 0;
  }

  const ::egxml::Files& Project::
  Files () const
  {
    return *this->Files_;
  }

  ::egxml::Files& Project::
  Files ()
  {
    return *this->Files_;
  }

  void Project::
  Files (::egxml::Files* x)
  {
    delete this->Files_;
    this->Files_ = x;
  }

  const Project::Build_sequence& Project::
  Build () const
  {
    return this->Build_;
  }

  Project::Build_sequence& Project::
  Build ()
  {
    return this->Build_;
  }

  const Project::Run_sequence& Project::
  Run () const
  {
    return this->Run_;
  }

  Project::Run_sequence& Project::
  Run ()
  {
    return this->Run_;
  }

  bool Project::
  Defaults_present () const
  {
    return this->Defaults_present_;
  }

  void Project::
  Defaults_present (bool x)
  {
    this->Defaults_present_ = x;
  }

  const ::egxml::Defaults& Project::
  Defaults () const
  {
    return this->Defaults_;
  }

  ::egxml::Defaults& Project::
  Defaults ()
  {
    return this->Defaults_;
  }

  void Project::
  Defaults (const ::egxml::Defaults& x)
  {
    this->Defaults_ = x;
    this->Defaults_present_ = true;
  }

  // Files
  //

  const Files::System_sequence& Files::
  System () const
  {
    return this->System_;
  }

  Files::System_sequence& Files::
  System ()
  {
    return this->System_;
  }

  const Files::Include_sequence& Files::
  Include () const
  {
    return this->Include_;
  }

  Files::Include_sequence& Files::
  Include ()
  {
    return this->Include_;
  }

  const Files::Source_sequence& Files::
  Source () const
  {
    return this->Source_;
  }

  Files::Source_sequence& Files::
  Source ()
  {
    return this->Source_;
  }

  const Files::Library_sequence& Files::
  Library () const
  {
    return this->Library_;
  }

  Files::Library_sequence& Files::
  Library ()
  {
    return this->Library_;
  }

  // EG
  //

  EG::choice_arm_tag EG::
  choice_arm () const
  {
    return this->choice_arm_;
  }

  const ::egxml::Package& EG::
  Package () const
  {
    return *this->choice_.Package_;
  }

  ::egxml::Package& EG::
  Package ()
  {
    return *this->choice_.Package_;
  }

  void EG::
  Package (::egxml::Package* x)
  {
    if (this->choice_arm_ != Package_tag)
      this->choice_arm (Package_tag);
    else
    {
      delete this->choice_.Package_;
    }

    this->choice_.Package_ = x;
  }

  const ::egxml::Host& EG::
  Host () const
  {
    return *this->choice_.Host_;
  }

  ::egxml::Host& EG::
  Host ()
  {
    return *this->choice_.Host_;
  }

  void EG::
  Host (::egxml::Host* x)
  {
    if (this->choice_arm_ != Host_tag)
      this->choice_arm (Host_tag);
    else
    {
      delete this->choice_.Host_;
    }

    this->choice_.Host_ = x;
  }

  const ::egxml::Project& EG::
  Project () const
  {
    return *this->choice_.Project_;
  }

  ::egxml::Project& EG::
  Project ()
  {
    return *this->choice_.Project_;
  }

  void EG::
  Project (::egxml::Project* x)
  {
    if (this->choice_arm_ != Project_tag)
      this->choice_arm (Project_tag);
    else
    {
      delete this->choice_.Project_;
    }

    this->choice_.Project_ = x;
  }

  // Directories
  //

  const Directories::Include_sequence& Directories::
  Include () const
  {
    return this->Include_;
  }

  Directories::Include_sequence& Directories::
  Include ()
  {
    return this->Include_;
  }

  const Directories::Library_sequence& Directories::
  Library () const
  {
    return this->Library_;
  }

  Directories::Library_sequence& Directories::
  Library ()
  {
    return this->Library_;
  }

  // Run
  //

  const ::std::string& Run::
  Name () const
  {
    return this->Name_;
  }

  ::std::string& Run::
  Name ()
  {
    return this->Name_;
  }

  void Run::
  Name (const ::std::string& x)
  {
    this->Name_ = x;
  }

  const ::std::string& Run::
  Command () const
  {
    return this->Command_;
  }

  ::std::string& Run::
  Command ()
  {
    return this->Command_;
  }

  void Run::
  Command (const ::std::string& x)
  {
    this->Command_ = x;
  }

  const Run::Argument_sequence& Run::
  Argument () const
  {
    return this->Argument_;
  }

  Run::Argument_sequence& Run::
  Argument ()
  {
    return this->Argument_;
  }
}

#include <stdlib.h>
#include <new>

namespace egxml
{
  // Package
  //

  Package::
  Package ()
  {
    this->Repository_present_ = false;
    this->Description_present_ = false;
    this->Directories_ = 0;
    this->Files_ = 0;
  }

  Package::
  ~Package ()
  {
    delete this->Directories_;
    delete this->Files_;
  }

  // Host
  //

  Host::
  Host ()
  {
    this->CompilerFlags_present_ = false;
    this->LinkerFlags_present_ = false;
  }

  Host::
  ~Host ()
  {
  }

  // Build
  //

  Build::
  Build ()
  {
  }

  Build::
  ~Build ()
  {
  }

  Build::
  Build (const Build& x)
  {
    XSDE_UNUSED (x);
    this->Name_ = x.Name_;
    this->CompilerFlags_ = x.CompilerFlags_;
    this->LinkerFlags_ = x.LinkerFlags_;
  }

  Build& Build::
  operator= (const Build& x)
  {
    XSDE_UNUSED (x);
    this->Name_ = x.Name_;
    this->CompilerFlags_ = x.CompilerFlags_;
    this->LinkerFlags_ = x.LinkerFlags_;
    return *this;
  }

  // Stack
  //

  Stack::
  Stack ()
  {
    this->Size_present_ = false;
  }

  Stack::
  ~Stack ()
  {
  }

  Stack::
  Stack (const Stack& x)
  {
    XSDE_UNUSED (x);
    this->Size_present_ = x.Size_present_;
    if (this->Size_present_)
      this->Size_ = x.Size_;
  }

  Stack& Stack::
  operator= (const Stack& x)
  {
    XSDE_UNUSED (x);
    this->Size_present_ = x.Size_present_;
    if (this->Size_present_)
      this->Size_ = x.Size_;
    return *this;
  }

  // Fibers
  //

  Fibers::
  Fibers ()
  {
    this->Stack_present_ = false;
  }

  Fibers::
  ~Fibers ()
  {
  }

  Fibers::
  Fibers (const Fibers& x)
  {
    XSDE_UNUSED (x);
    this->Stack_present_ = x.Stack_present_;
    if (this->Stack_present_)
      this->Stack_ = x.Stack_;
  }

  Fibers& Fibers::
  operator= (const Fibers& x)
  {
    XSDE_UNUSED (x);
    this->Stack_present_ = x.Stack_present_;
    if (this->Stack_present_)
      this->Stack_ = x.Stack_;
    return *this;
  }

  // Defaults
  //

  Defaults::
  Defaults ()
  {
    this->Fibers_present_ = false;
  }

  Defaults::
  ~Defaults ()
  {
  }

  Defaults::
  Defaults (const Defaults& x)
  {
    XSDE_UNUSED (x);
    this->Fibers_present_ = x.Fibers_present_;
    if (this->Fibers_present_)
      this->Fibers_ = x.Fibers_;
  }

  Defaults& Defaults::
  operator= (const Defaults& x)
  {
    XSDE_UNUSED (x);
    this->Fibers_present_ = x.Fibers_present_;
    if (this->Fibers_present_)
      this->Fibers_ = x.Fibers_;
    return *this;
  }

  // Project
  //

  Project::
  Project ()
  {
    this->Description_present_ = false;
    this->Files_ = 0;
    this->Defaults_present_ = false;
  }

  Project::
  ~Project ()
  {
    delete this->Files_;
  }

  // Files
  //

  Files::
  Files ()
  {
  }

  Files::
  ~Files ()
  {
  }

  // EG
  //

  EG::
  EG ()
  {
    this->choice_arm_ = choice_arm_tag (3);
  }

  EG::
  ~EG ()
  {
    this->choice_arm (choice_arm_tag (3));
  }

  void EG::
  choice_arm (choice_arm_tag x)
  {
    if (this->choice_arm_ == x)
      return;

    switch (this->choice_arm_)
    {
      case Package_tag:
      {
        delete this->choice_.Package_;
        break;
      }
      case Host_tag:
      {
        delete this->choice_.Host_;
        break;
      }
      case Project_tag:
      {
        delete this->choice_.Project_;
        break;
      }
      default:
      break;
    }

    this->choice_arm_ = choice_arm_tag (3);

    switch (x)
    {
      case Package_tag:
      {
        this->choice_.Package_ = 0;
        break;
      }
      case Host_tag:
      {
        this->choice_.Host_ = 0;
        break;
      }
      case Project_tag:
      {
        this->choice_.Project_ = 0;
        break;
      }
      default:
      break;
    }

    this->choice_arm_ = x;
  }

  // Directories
  //

  Directories::
  Directories ()
  {
  }

  Directories::
  ~Directories ()
  {
  }

  // Run
  //

  Run::
  Run ()
  {
  }

  Run::
  ~Run ()
  {
  }
}

// Begin epilogue.
//
//
// End epilogue.

#include <xsde/cxx/post.hxx>

