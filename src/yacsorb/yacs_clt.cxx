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

#include <Python.h>
#include <yacs.hh>
#include "RuntimeSALOME.hxx"
#include "Proc.hxx"
#include "parsers.hxx"

#include <vector>
#include <string>
#include <iostream>
#include <pthread.h>

YACS::YACSLoader::YACSLoader* loader;

class Obs_i : public POA_YACS_ORB::Observer,
              public PortableServer::RefCountServantBase
{
public:
  inline Obs_i(YACS_ORB::Proc_ptr proc) {_server_proc=proc;}
  virtual ~Obs_i() {}
  void notifyObserver(CORBA::Long numid, const char* event);
protected:
  YACS_ORB::Proc_ptr _server_proc;
};

void Obs_i::notifyObserver(CORBA::Long numid, const char* event)
{
  std::cerr << "Obs_i::notifyObserver " << numid << event << std::endl;
  std::cerr << "Obs_i::notifyObserver:state= " << _server_proc->getState(numid) << std::endl;
  std::cerr << "Obs_i::notifyObserver:XMLstate= " << _server_proc->getXMLState(numid) << std::endl;
}

YACS_ORB::YACS_Gen_var yacsref;
YACS_ORB::Proc_ptr server_proc;

void * run(void *obj)
{
    yacsref->Run(server_proc);
}

int main(int argc, char** argv)
{
  YACS::ENGINE::RuntimeSALOME::setRuntime();
  loader= new YACS::YACSLoader::YACSLoader();

  try 
  {
    CORBA::ORB_var orb = CORBA::ORB_init(argc, argv);
    CORBA::Object_var obj = orb->string_to_object("corbaname:rir:#test.my_context/Yacs.Object");
    yacsref = YACS_ORB::YACS_Gen::_narrow(obj);
    if( CORBA::is_nil(yacsref) ) 
      {
        std::cerr << "Can't narrow reference to type yacs (or it was nil)." << std::endl;
        return 1;
      }

    //Activate POA
    CORBA::Object_var poa = orb->resolve_initial_references("RootPOA");
    PortableServer::POA_var root_poa = PortableServer::POA::_narrow(poa);
    PortableServer::POAManager_var poa_man = root_poa->the_POAManager();
    poa_man->activate();

    char* xmlFile="/local/chris/SALOME2/SUPERV/YACS/BR_CC/YACS_SRC/src/yacsloader/samples/aschema.xml";
    //Load XML file in client
    YACS::ENGINE::Proc* local_proc=loader->load(xmlFile);
    //Load xml file in server
    server_proc = yacsref->Load(xmlFile);
    //Create an observer for server_proc
    Obs_i* obs=new Obs_i(server_proc);
    YACS_ORB::Observer_ptr obs_ptr = obs->_this();
    //Get ids and names
    YACS_ORB::stringArray_var names;
    YACS_ORB::longArray_var ids;
    server_proc->getIds(ids.out(),names.out());
    //Register it 
    int len=ids->length();
    int numid;
    for(int i=0;i<len;i++)
      {
        numid=ids[i];
        yacsref->addObserver(obs_ptr,numid,"status");
      }
    //Execute Proc in thread ??
    pthread_t th;
    //pthread_create(&th,NULL,run,0);
    yacsref->Run(server_proc);

    //orb->run();
    orb->destroy();
  }
  catch(CORBA::COMM_FAILURE& ex) {
      std::cerr << "Caught system exception COMM_FAILURE -- unable to contact the "
         << "object." << std::endl;
  }
  catch(CORBA::SystemException&) {
      std::cerr << "Caught a CORBA::SystemException." << std::endl;
  }
  catch(CORBA::Exception&) {
      std::cerr << "Caught CORBA::Exception." << std::endl;
  }
  catch(omniORB::fatalException& fe) {
      std::cerr << "Caught omniORB::fatalException:" << std::endl;
      std::cerr << "  file: " << fe.file() << std::endl;
      std::cerr << "  line: " << fe.line() << std::endl;
      std::cerr << "  mesg: " << fe.errmsg() << std::endl;
  }
  catch(...) {
      std::cerr << "Caught unknown exception." << std::endl;
  }
  return 0;
}
