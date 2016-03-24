// Copyright (C) 2006-2016  CEA/DEN, EDF R&D
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

#ifdef USE_CPPUNIT
#undef USE_CPPUNIT
#endif

#include "runtimeTest.hxx"

int main()
{
  YACS::RuntimeTest T;
  T.setUp();
  T.initRuntimeTypeCode();
  T.createPythonNodes();
  T.createCORBANodes();
  T.createBloc();
  T.createRecursiveBlocs();
  T.createDataLinks();
  T.createPythonNodesWithScript();
  T.createCORBANodesWithMethod();
  T.createXMLNodes();
  T.createBloc2();
  T.createDataLinksPythonPython();
  /*
  T.createDataLinksPythonCORBA();
  T.createDataLinksCORBACORBA();
  T.createDataLinksCORBAPython();
  T.createDataLinksXML();
  T.manualInitInputPort();
  T.manualExecuteNoThread();
  T.manualGetOutputs();
  T.createCppNodes();
  T.convertPorts();
  T.executeCppNode();
  T.createGraphWithCppNodes();
  T.classTeardown();
*/  
  return 0;
}

