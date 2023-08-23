// Copyright (C) 2006-2023  CEA, EDF
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

#define UNIT_TEST_HEADER " --- TEST src/yacsloader"

#include "YacsLoaderTest.hxx"
#include "RuntimeSALOME.hxx"
#include "PythonCppUtils.hxx"

#if SALOME_KERNEL
#include "SALOME_Embedded_NamingService.hxx"
#include "SALOME_ContainerManager.hxx"
#include "SALOME_NamingService_Wrapper.hxx"
#endif

using namespace YACS;
using namespace std;

// --- Registers the fixture into the 'registry'

CPPUNIT_TEST_SUITE_REGISTRATION( YacsLoaderTest );

// --- generic Main program from bases/Test

#include "BasicMainTestInternal.hxx"
#include "RuntimeSALOME.hxx"

#include <chrono>
#include <thread>

int main()
{
  YACS::ENGINE::RuntimeSALOME::setRuntime();
  YACS::ENGINE::RuntimeSALOME *rt = YACS::ENGINE::getSALOMERuntime();
  if( !rt )
    return 1;
  Engines::EmbeddedNamingService_var ns = GetEmbeddedNamingService();
  CORBA::ORB_ptr orb = rt->getOrb();
  CORBA::String_var ior = orb->object_to_string( ns );
  AutoPyRef proc = rt->launchSubProcess({"./echoSrv",std::string(ior)});
  //std::cout << getpid() << std::endl;
  std::this_thread::sleep_for(std::chrono::microseconds(3000000));
  int ret = BasicMainTestInternal();
  //
  SALOME_NamingService_Wrapper namingService(orb);
  CORBA::Object_var objDSM(namingService.Resolve(SALOME_ContainerManager::_ContainerManagerNameInNS));
  Engines::ContainerManager_var dsm(Engines::ContainerManager::_narrow(objDSM));
  dsm->ShutdownContainers();
  //
  AutoGIL agil;
  AutoPyRef terminateStr = PyUnicode_FromString("terminate");
  AutoPyRef dummy = PyObject_CallMethodObjArgs(proc,terminateStr,nullptr);
  return ret;
}
