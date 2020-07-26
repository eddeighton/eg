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

#include "eg_schema-simpl.hxx"

#include <xsde/cxx/serializer/validating/string-common.hxx>

namespace egxml
{
  // Package_simpl
  //

  void Package_simpl::
  pre (const ::egxml::Package& x)
  {
    this->Package_simpl_state_.Package_ = &x;
    this->Package_simpl_state_.Command_ = 
    this->Package_simpl_state_.Package_->Command ().begin ();
    this->Package_simpl_state_.Command_end_ = 
    this->Package_simpl_state_.Package_->Command ().end ();
  }

  ::std::string Package_simpl::
  Name ()
  {
    return this->Package_simpl_state_.Package_->Name ();
  }

  bool Package_simpl::
  Repository_present ()
  {
    return this->Package_simpl_state_.Package_->Repository_present ();
  }

  ::std::string Package_simpl::
  Repository ()
  {
    return this->Package_simpl_state_.Package_->Repository ();
  }

  ::std::string Package_simpl::
  License ()
  {
    return this->Package_simpl_state_.Package_->License ();
  }

  bool Package_simpl::
  Description_present ()
  {
    return this->Package_simpl_state_.Package_->Description_present ();
  }

  ::std::string Package_simpl::
  Description ()
  {
    return this->Package_simpl_state_.Package_->Description ();
  }

  bool Package_simpl::
  Directories_present ()
  {
    return this->Package_simpl_state_.Package_->Directories_present ();
  }

  const ::egxml::Directories& Package_simpl::
  Directories ()
  {
    return this->Package_simpl_state_.Package_->Directories ();
  }

  bool Package_simpl::
  Files_present ()
  {
    return this->Package_simpl_state_.Package_->Files_present ();
  }

  const ::egxml::Files& Package_simpl::
  Files ()
  {
    return this->Package_simpl_state_.Package_->Files ();
  }

  bool Package_simpl::
  Command_next ()
  {
    return this->Package_simpl_state_.Command_ != 
    this->Package_simpl_state_.Command_end_;
  }

  ::std::string Package_simpl::
  Command ()
  {
    return *this->Package_simpl_state_.Command_++;
  }

  // Host_simpl
  //

  Host_simpl::
  Host_simpl ()
  : Host_sskel (&base_impl_)
  {
  }

  void Host_simpl::
  pre (const ::egxml::Host& x)
  {
    this->base_impl_.pre (x);
    this->Host_simpl_state_.Host_ = &x;
  }

  bool Host_simpl::
  CompilerFlags_present ()
  {
    return this->Host_simpl_state_.Host_->CompilerFlags_present ();
  }

  ::std::string Host_simpl::
  CompilerFlags ()
  {
    return this->Host_simpl_state_.Host_->CompilerFlags ();
  }

  bool Host_simpl::
  LinkerFlags_present ()
  {
    return this->Host_simpl_state_.Host_->LinkerFlags_present ();
  }

  ::std::string Host_simpl::
  LinkerFlags ()
  {
    return this->Host_simpl_state_.Host_->LinkerFlags ();
  }

  // Build_simpl
  //

  void Build_simpl::
  pre (const ::egxml::Build& x)
  {
    this->Build_simpl_state_.Build_ = &x;
  }

  ::std::string Build_simpl::
  Name ()
  {
    return this->Build_simpl_state_.Build_->Name ();
  }

  ::std::string Build_simpl::
  CompilerFlags ()
  {
    return this->Build_simpl_state_.Build_->CompilerFlags ();
  }

  ::std::string Build_simpl::
  LinkerFlags ()
  {
    return this->Build_simpl_state_.Build_->LinkerFlags ();
  }

  // Stack_simpl
  //

  void Stack_simpl::
  pre (const ::egxml::Stack& x)
  {
    this->Stack_simpl_state_.Stack_ = &x;
  }

  bool Stack_simpl::
  Size_present ()
  {
    return this->Stack_simpl_state_.Stack_->Size_present ();
  }

  unsigned int Stack_simpl::
  Size ()
  {
    return this->Stack_simpl_state_.Stack_->Size ();
  }

  // Fibers_simpl
  //

  void Fibers_simpl::
  pre (const ::egxml::Fibers& x)
  {
    this->Fibers_simpl_state_.Fibers_ = &x;
  }

  bool Fibers_simpl::
  Stack_present ()
  {
    return this->Fibers_simpl_state_.Fibers_->Stack_present ();
  }

  const ::egxml::Stack& Fibers_simpl::
  Stack ()
  {
    return this->Fibers_simpl_state_.Fibers_->Stack ();
  }

  // Defaults_simpl
  //

  void Defaults_simpl::
  pre (const ::egxml::Defaults& x)
  {
    this->Defaults_simpl_state_.Defaults_ = &x;
  }

  bool Defaults_simpl::
  Fibers_present ()
  {
    return this->Defaults_simpl_state_.Defaults_->Fibers_present ();
  }

  const ::egxml::Fibers& Defaults_simpl::
  Fibers ()
  {
    return this->Defaults_simpl_state_.Defaults_->Fibers ();
  }

  // Project_simpl
  //

  void Project_simpl::
  pre (const ::egxml::Project& x)
  {
    this->Project_simpl_state_.Project_ = &x;
    this->Project_simpl_state_.Package_ = 
    this->Project_simpl_state_.Project_->Package ().begin ();
    this->Project_simpl_state_.Package_end_ = 
    this->Project_simpl_state_.Project_->Package ().end ();
    this->Project_simpl_state_.Build_ = 
    this->Project_simpl_state_.Project_->Build ().begin ();
    this->Project_simpl_state_.Build_end_ = 
    this->Project_simpl_state_.Project_->Build ().end ();
    this->Project_simpl_state_.Run_ = 
    this->Project_simpl_state_.Project_->Run ().begin ();
    this->Project_simpl_state_.Run_end_ = 
    this->Project_simpl_state_.Project_->Run ().end ();
  }

  ::std::string Project_simpl::
  Name ()
  {
    return this->Project_simpl_state_.Project_->Name ();
  }

  ::std::string Project_simpl::
  Host ()
  {
    return this->Project_simpl_state_.Project_->Host ();
  }

  bool Project_simpl::
  Description_present ()
  {
    return this->Project_simpl_state_.Project_->Description_present ();
  }

  ::std::string Project_simpl::
  Description ()
  {
    return this->Project_simpl_state_.Project_->Description ();
  }

  bool Project_simpl::
  Package_next ()
  {
    return this->Project_simpl_state_.Package_ != 
    this->Project_simpl_state_.Package_end_;
  }

  ::std::string Project_simpl::
  Package ()
  {
    return *this->Project_simpl_state_.Package_++;
  }

  bool Project_simpl::
  Files_present ()
  {
    return this->Project_simpl_state_.Project_->Files_present ();
  }

  const ::egxml::Files& Project_simpl::
  Files ()
  {
    return this->Project_simpl_state_.Project_->Files ();
  }

  bool Project_simpl::
  Build_next ()
  {
    return this->Project_simpl_state_.Build_ != 
    this->Project_simpl_state_.Build_end_;
  }

  const ::egxml::Build& Project_simpl::
  Build ()
  {
    return *this->Project_simpl_state_.Build_++;
  }

  bool Project_simpl::
  Run_next ()
  {
    return this->Project_simpl_state_.Run_ != 
    this->Project_simpl_state_.Run_end_;
  }

  const ::egxml::Run& Project_simpl::
  Run ()
  {
    return *this->Project_simpl_state_.Run_++;
  }

  bool Project_simpl::
  Defaults_present ()
  {
    return this->Project_simpl_state_.Project_->Defaults_present ();
  }

  const ::egxml::Defaults& Project_simpl::
  Defaults ()
  {
    return this->Project_simpl_state_.Project_->Defaults ();
  }

  // Files_simpl
  //

  void Files_simpl::
  pre (const ::egxml::Files& x)
  {
    this->Files_simpl_state_.Files_ = &x;
    this->Files_simpl_state_.System_ = 
    this->Files_simpl_state_.Files_->System ().begin ();
    this->Files_simpl_state_.System_end_ = 
    this->Files_simpl_state_.Files_->System ().end ();
    this->Files_simpl_state_.Include_ = 
    this->Files_simpl_state_.Files_->Include ().begin ();
    this->Files_simpl_state_.Include_end_ = 
    this->Files_simpl_state_.Files_->Include ().end ();
    this->Files_simpl_state_.Source_ = 
    this->Files_simpl_state_.Files_->Source ().begin ();
    this->Files_simpl_state_.Source_end_ = 
    this->Files_simpl_state_.Files_->Source ().end ();
    this->Files_simpl_state_.Library_ = 
    this->Files_simpl_state_.Files_->Library ().begin ();
    this->Files_simpl_state_.Library_end_ = 
    this->Files_simpl_state_.Files_->Library ().end ();
  }

  bool Files_simpl::
  System_next ()
  {
    return this->Files_simpl_state_.System_ != 
    this->Files_simpl_state_.System_end_;
  }

  ::std::string Files_simpl::
  System ()
  {
    return *this->Files_simpl_state_.System_++;
  }

  bool Files_simpl::
  Include_next ()
  {
    return this->Files_simpl_state_.Include_ != 
    this->Files_simpl_state_.Include_end_;
  }

  ::std::string Files_simpl::
  Include ()
  {
    return *this->Files_simpl_state_.Include_++;
  }

  bool Files_simpl::
  Source_next ()
  {
    return this->Files_simpl_state_.Source_ != 
    this->Files_simpl_state_.Source_end_;
  }

  ::std::string Files_simpl::
  Source ()
  {
    return *this->Files_simpl_state_.Source_++;
  }

  bool Files_simpl::
  Library_next ()
  {
    return this->Files_simpl_state_.Library_ != 
    this->Files_simpl_state_.Library_end_;
  }

  ::std::string Files_simpl::
  Library ()
  {
    return *this->Files_simpl_state_.Library_++;
  }

  // EG_simpl
  //

  void EG_simpl::
  pre (const ::egxml::EG& x)
  {
    this->EG_simpl_state_.EG_ = &x;
  }

  EG_sskel::choice_arm_tag EG_simpl::
  choice_arm ()
  {
    choice_arm_tag t (static_cast< choice_arm_tag > (
                        this->EG_simpl_state_.EG_->choice_arm ()));
    return t;
  }

  const ::egxml::Package& EG_simpl::
  Package ()
  {
    return this->EG_simpl_state_.EG_->Package ();
  }

  const ::egxml::Host& EG_simpl::
  Host ()
  {
    return this->EG_simpl_state_.EG_->Host ();
  }

  const ::egxml::Project& EG_simpl::
  Project ()
  {
    return this->EG_simpl_state_.EG_->Project ();
  }

  // Directories_simpl
  //

  void Directories_simpl::
  pre (const ::egxml::Directories& x)
  {
    this->Directories_simpl_state_.Directories_ = &x;
    this->Directories_simpl_state_.Include_ = 
    this->Directories_simpl_state_.Directories_->Include ().begin ();
    this->Directories_simpl_state_.Include_end_ = 
    this->Directories_simpl_state_.Directories_->Include ().end ();
    this->Directories_simpl_state_.Library_ = 
    this->Directories_simpl_state_.Directories_->Library ().begin ();
    this->Directories_simpl_state_.Library_end_ = 
    this->Directories_simpl_state_.Directories_->Library ().end ();
  }

  bool Directories_simpl::
  Include_next ()
  {
    return this->Directories_simpl_state_.Include_ != 
    this->Directories_simpl_state_.Include_end_;
  }

  ::std::string Directories_simpl::
  Include ()
  {
    return *this->Directories_simpl_state_.Include_++;
  }

  bool Directories_simpl::
  Library_next ()
  {
    return this->Directories_simpl_state_.Library_ != 
    this->Directories_simpl_state_.Library_end_;
  }

  ::std::string Directories_simpl::
  Library ()
  {
    return *this->Directories_simpl_state_.Library_++;
  }

  // Run_simpl
  //

  void Run_simpl::
  pre (const ::egxml::Run& x)
  {
    this->Run_simpl_state_.Run_ = &x;
    this->Run_simpl_state_.Argument_ = 
    this->Run_simpl_state_.Run_->Argument ().begin ();
    this->Run_simpl_state_.Argument_end_ = 
    this->Run_simpl_state_.Run_->Argument ().end ();
  }

  ::std::string Run_simpl::
  Name ()
  {
    return this->Run_simpl_state_.Run_->Name ();
  }

  ::std::string Run_simpl::
  Command ()
  {
    return this->Run_simpl_state_.Run_->Command ();
  }

  bool Run_simpl::
  Argument_next ()
  {
    return this->Run_simpl_state_.Argument_ != 
    this->Run_simpl_state_.Argument_end_;
  }

  ::std::string Run_simpl::
  Argument ()
  {
    return *this->Run_simpl_state_.Argument_++;
  }
}

namespace egxml
{
  // EG_saggr
  //

  EG_saggr::
  EG_saggr ()
  {
    this->Package_s_.serializers (this->string_s_,
                                  this->string_s_,
                                  this->string_s_,
                                  this->string_s_,
                                  this->Directories_s_,
                                  this->Files_s_,
                                  this->string_s_);

    this->Directories_s_.serializers (this->string_s_,
                                      this->string_s_);

    this->Host_s_.serializers (this->string_s_,
                               this->string_s_,
                               this->string_s_,
                               this->string_s_,
                               this->Directories_s_,
                               this->Files_s_,
                               this->string_s_,
                               this->string_s_,
                               this->string_s_);

    this->Project_s_.serializers (this->string_s_,
                                  this->string_s_,
                                  this->string_s_,
                                  this->string_s_,
                                  this->Files_s_,
                                  this->Build_s_,
                                  this->Run_s_,
                                  this->Defaults_s_);

    this->Build_s_.serializers (this->string_s_,
                                this->string_s_,
                                this->string_s_);

    this->Run_s_.serializers (this->string_s_,
                              this->string_s_,
                              this->string_s_);

    this->Defaults_s_.serializers (this->Fibers_s_);

    this->Fibers_s_.serializers (this->Stack_s_);

    this->Stack_s_.serializers (this->unsigned_int_s_);

    this->Files_s_.serializers (this->string_s_,
                                this->string_s_,
                                this->string_s_,
                                this->string_s_);

    this->EG_s_.serializers (this->Package_s_,
                             this->Host_s_,
                             this->Project_s_);
  }

  const char* EG_saggr::
  root_name ()
  {
    return "EG";
  }

  const char* EG_saggr::
  root_namespace ()
  {
    return "";
  }
}

// Begin epilogue.
//
//
// End epilogue.

