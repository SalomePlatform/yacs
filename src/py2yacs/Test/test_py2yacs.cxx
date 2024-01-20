// Copyright (C) 2017-2024  CEA, EDF
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
#include <iostream>
#include <fstream>
#include <sstream>
#include <cerrno>
#include "py2yacs.hxx"
#include "RuntimeSALOME.hxx"

/*
#include "Proc.hxx"
#include "Node.hxx"
#include "CORBAPorts.hxx"
#include "InputPort.hxx"
#include "Executor.hxx"
*/

int main(int argc, char *argv[])
{
  if (argc != 4)
  {
    std::cerr << "Usage: " <<  argv[0] << " pythonfile funcname outfile" << std::endl;
    return 1;
  }
  YACS::ENGINE::RuntimeSALOME::setRuntime();
  //Py_Initialize();
  Py2yacs parser;
  std::ifstream file_stream(argv[1]);
  if(!file_stream)
  {
    std::cerr << "Error when opening file " << argv[1]
             << ": " << strerror(errno)
             << std::endl;
    return 1;
  }
  std::stringstream buffer;
  buffer << file_stream.rdbuf();
  try
  {
    parser.load(buffer.str());
    parser.save(argv[3], argv[2]);
    
    /*
    YACS::ENGINE::Proc* p = parser.createProc(argv[2]);
    //p->setInPortValue("Schema.default_name", "a", "42.");
    YACS::ENGINE::Node* n = p->getChildByShortName("default_name");
    //YACS::ENGINE::Node* n = p->nodeMap["default_name"];
    if(!n)
      std::cerr << "Node not found." << std::endl;
    else
      n->getInputPort("a")->edInit(42.);
    p->saveSchema(argv[3]);
    
    YACS::ENGINE::Executor executor;
    executor.RunW(p, 0);
    n->getOutputPort("x")->dump();
    */
  }
  catch(Py2yacsException& e)
  {
    std::cerr << e.what() << std::endl;
  }
  YACS::ENGINE::getRuntime()->fini();
  //Py_Finalize();
}