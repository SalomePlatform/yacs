// Copyright (C) 2021-2024  CEA, EDF
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#pragma once

#include "SALOME_NamingService_Abstract.hxx"
#include "YACSRuntimeSALOMEExport.hxx"

#include <memory>

/*!
 * Decorator class that allows YACS engine to switch between :
 * - NamingService CORBA server (SALOME_NamingService)
 * - NamingService embedded (SALOME_Fake_NamingService)
 */
class YACSRUNTIMESALOME_EXPORT SALOME_NamingService_Wrapper : public SALOME_NamingService_Abstract
{
public:
  SALOME_NamingService_Wrapper();
  SALOME_NamingService_Wrapper(CORBA::ORB_ptr orb);
  std::vector< std::string > repr() override { return _effective_ns->repr(); }
  void init_orb(CORBA::ORB_ptr orb=0) override { _effective_ns->init_orb(orb); }
  bool IsTrueNS() const override { return _effective_ns->IsTrueNS(); }
  void Register(CORBA::Object_ptr ObjRef, const char* Path) override { _effective_ns->Register(ObjRef,Path); }
  CORBA::Object_ptr Resolve(const char* Path) override { return _effective_ns->Resolve(Path); }
  CORBA::Object_ptr ResolveFirst(const char* Path) override { return _effective_ns->ResolveFirst(Path); }
  void Destroy_Name(const char* Path) override { _effective_ns->Destroy_Name(Path); }
  void Destroy_Directory(const char* Path) override { _effective_ns->Destroy_Directory(Path); }
  void Destroy_FullDirectory(const char* Path) override { _effective_ns->Destroy_FullDirectory(Path); }
  bool Change_Directory(const char* Path) override { return _effective_ns->Change_Directory(Path); }
  std::vector<std::string> list_subdirs() override { return _effective_ns->list_directory(); }
  std::vector<std::string> list_directory() override { return _effective_ns->list_directory(); }
  std::vector<std::string> list_directory_recurs() override { return _effective_ns->list_directory_recurs(); }
  SALOME_NamingService_Abstract *clone() override { return new SALOME_NamingService_Wrapper(*this); }
  CORBA::Object_ptr ResolveComponent(const char* hostname, const char* containerName, const char* componentName, const int nbproc=0) override { return _effective_ns->ResolveComponent(hostname,containerName,componentName,nbproc); }
private:
  SALOME_NamingService_Wrapper(const SALOME_NamingService_Wrapper& other);
  void initializeEffectiveNS();
private:
  std::unique_ptr<SALOME_NamingService_Abstract> _effective_ns;
};
