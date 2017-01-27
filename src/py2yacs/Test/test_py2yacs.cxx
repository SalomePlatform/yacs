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