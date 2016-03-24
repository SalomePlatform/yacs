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
#include "Exception.hxx"
#include "Executor.hxx"
#include "Dispatcher.hxx"
#include "parsers.hxx"

#include <iostream>
#include <sstream>
#include <set>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;

YACS::YACSLoader::YACSLoader* loader;
CORBA::ORB_ptr orb;
YACS_ORB::YACS_Gen_var myyacsref;

class MyDispatcher:public YACS::ENGINE::Dispatcher
{
public:
  void dispatch(YACS::ENGINE::Node* object, const std::string& event)
  {
    std::cerr << "dispatch " << object->getNumId() << std::endl;
    typedef std::set<YACS_ORB::Observer_ptr>::iterator jt;
    std::pair<int,std::string> key(object->getNumId(),event);
    for(jt iter=_observers[key].begin();iter!=_observers[key].end();iter++)
      {
        (*iter)->notifyObserver((CORBA::Long)object->getNumId(),event.c_str());
      }
  }

  void addObserver(YACS_ORB::Observer_ptr observer,int numid, const std::string& event)
  {
    _observers[std::pair<int,std::string>(numid,event)].insert(YACS_ORB::Observer::_duplicate(observer));
  //  printObservers();
  }
protected:
  std::map< std::pair<int,std::string> , std::set<YACS_ORB::Observer_ptr> > _observers;
};

class Yacs_i : public POA_YACS_ORB::YACS_Gen,
               public PortableServer::RefCountServantBase
{
public:
  inline Yacs_i() {}
  virtual ~Yacs_i() {}
  YACS_ORB::Proc_ptr Load(const char* xmlFile);
  void Run(YACS_ORB::Proc_ptr p);
  void addObserver(YACS_ORB::Observer_ptr observer, CORBA::Long numid,const char* event);
};

class Proc_i : public POA_YACS_ORB::Proc,
               public PortableServer::RefCountServantBase
{
public:
  inline Proc_i(YACS::ENGINE::Proc* p) {_proc=p;};
  virtual ~Proc_i() {};
  virtual void RunW();
  CORBA::Long getState(CORBA::Long numid);
  char * getXMLState(CORBA::Long numid);
  void getIds(YACS_ORB::longArray_out numids,YACS_ORB::stringArray_out names);
protected:
  YACS::ENGINE::Executor _executor;
  YACS::ENGINE::Proc* _proc;
};

void Proc_i::RunW()
{
  _executor.RunW(_proc,0);
}

CORBA::Long Proc_i::getState(CORBA::Long numid)
{
  if(YACS::ENGINE::Node::idMap.count(numid) == 0)
    {
      std::cerr << "Unknown node id " << numid << std::endl;
      return (CORBA::Long)-1;
    }
  YACS::ENGINE::Node* node= YACS::ENGINE::Node::idMap[numid];
  CORBA::Long state=node->getEffectiveState();
  return state;
}

char * Proc_i::getXMLState(CORBA::Long numid)
{
  if(YACS::ENGINE::Node::idMap.count(numid) == 0)
    {
      std::cerr << "Unknown node id " << numid << std::endl;
      return "<state>unknown</state>";
    }
  YACS::ENGINE::Node* node= YACS::ENGINE::Node::idMap[numid];
  std::stringstream msg;
  msg << "<state>" << node->getEffectiveState() << "</state>";
  msg << "<name>" << node->getQualifiedName() << "</name>";
  msg << "<id>" << numid << "</id>";
  return CORBA::string_dup(msg.str().c_str());
}

void Proc_i::getIds(YACS_ORB::longArray_out numids,YACS_ORB::stringArray_out names)
{
  std::list<YACS::ENGINE::Node *> nodes=_proc->getAllRecursiveNodes();
  int len=nodes.size();
  names=new YACS_ORB::stringArray;
  numids=new YACS_ORB::longArray;
  names->length(len);
  numids->length(len);
  int i=0;
  for(list<YACS::ENGINE::Node *>::const_iterator iter=nodes.begin();iter!=nodes.end();iter++,i++)
    {
      (*names)[i]=CORBA::string_dup((*iter)->getQualifiedName().c_str());
      (*numids)[i]=(*iter)->getNumId();
    }
}

YACS_ORB::Proc_ptr Yacs_i::Load(const char* xmlFile)
{
  YACS::ENGINE::Proc* proc=loader->load(xmlFile);
  Proc_i* p=new Proc_i(proc);
  YACS_ORB::Proc_ptr pp = p->_this();
  return pp;
}

void Yacs_i::addObserver(YACS_ORB::Observer_ptr observer, CORBA::Long numid,const char* event)
{
  ((MyDispatcher*)YACS::ENGINE::Dispatcher::getDispatcher())->addObserver(observer,numid,event);
}

void Yacs_i::Run(YACS_ORB::Proc_ptr p)
{
  Proc_i *servant=dynamic_cast<Proc_i *> (PortableServer::POA::_the_root_poa()->reference_to_servant(p));
  servant->RunW();
}

static CORBA::Boolean bindObjectToName(CORBA::ORB_ptr, CORBA::Object_ptr,const char*);
static ostream& operator<<(ostream& os, const CORBA::Exception& e);

int main(int argc, char** argv)
{
  YACS::ENGINE::RuntimeSALOME::setRuntime();
  loader= new YACS::YACSLoader::YACSLoader();
  MyDispatcher* disp=new MyDispatcher();
  YACS::ENGINE::Dispatcher::setDispatcher(disp);

  try 
  {
    orb = CORBA::ORB_init(argc, argv);

    {
      CORBA::Object_var obj = orb->resolve_initial_references("RootPOA");
      PortableServer::POA_var root_poa = PortableServer::POA::_narrow(obj);
      // POA manager
      PortableServer::POAManager_var poa_man = root_poa->the_POAManager();
      poa_man->activate();

      // Create and activate servant
      Yacs_i* myyacs = new Yacs_i();
      // Obtain a reference to the object, and print it out as a
      // stringified IOR.
      obj = myyacs->_this();
      CORBA::String_var sior(orb->object_to_string(obj));
      DEBTRACE("'" << (char*)sior << "'");
      myyacsref = YACS_ORB::YACS_Gen::_narrow(obj);

      if( !bindObjectToName(orb, myyacsref,"Yacs") ) return 1;

      // Decrement the reference count of the object implementation, so
      // that it will be properly cleaned up when the POA has determined
      // that it is no longer needed.
      myyacs->_remove_ref();
    }
    orb->run();
  }
  catch(CORBA::SystemException&) {
    DEBTRACE("Caught CORBA::SystemException.");
  }
  catch(CORBA::Exception& ex) {
    DEBTRACE("Caught CORBA::Exception." << ex);
  }
  catch(omniORB::fatalException& fe) {
    DEBTRACE("Caught omniORB::fatalException:");
    DEBTRACE("  file: " << fe.file());
    DEBTRACE("  line: " << fe.line());
    DEBTRACE("  mesg: " << fe.errmsg());
  }
  catch(...) {
    DEBTRACE("Caught unknown exception." );
  }

  return 0;
}


//////////////////////////////////////////////////////////////////////

static CORBA::Boolean
bindObjectToName(CORBA::ORB_ptr orb, CORBA::Object_ptr objref,const char *name)
{
  CosNaming::NamingContext_var rootContext;

  try {
    // Obtain a reference to the root context of the Name service:
    CORBA::Object_var obj;
    obj = orb->resolve_initial_references("NameService");

    // Narrow the reference returned.
    rootContext = CosNaming::NamingContext::_narrow(obj);
    if( CORBA::is_nil(rootContext) ) {
      DEBTRACE("Failed to narrow the root naming context.");
      return 0;
    }
  }
  catch(CORBA::ORB::InvalidName& ex) {
    // This should not happen!
    DEBTRACE("Service required is invalid [does not exist]." );
    return 0;
  }

  try {
    // Bind a context called "test" to the root context:

    CosNaming::Name contextName;
    contextName.length(1);
    contextName[0].id   = (const char*) "test";       // string copied
    contextName[0].kind = (const char*) "my_context"; // string copied
    // Note on kind: The kind field is used to indicate the type
    // of the object. This is to avoid conventions such as that used
    // by files (name.type -- e.g. test.ps = postscript etc.)

    CosNaming::NamingContext_var testContext;
    try {
      // Bind the context to root.
      testContext = rootContext->bind_new_context(contextName);
    }
    catch(CosNaming::NamingContext::AlreadyBound& ex) {
      // If the context already exists, this exception will be raised.
      // In this case, just resolve the name and assign testContext
      // to the object returned:
      CORBA::Object_var obj;
      obj = rootContext->resolve(contextName);
      testContext = CosNaming::NamingContext::_narrow(obj);
      if( CORBA::is_nil(testContext) ) {
        DEBTRACE("Failed to narrow naming context.");
        return 0;
      }
    }

    // Bind objref with name name to the testContext:
    CosNaming::Name objectName;
    objectName.length(1);
    objectName[0].id   = name;   // string copied
    objectName[0].kind = (const char*) "Object"; // string copied

    try {
      testContext->bind(objectName, objref);
    }
    catch(CosNaming::NamingContext::AlreadyBound& ex) {
      testContext->rebind(objectName, objref);
    }
  }
  catch(CORBA::COMM_FAILURE& ex) {
    DEBTRACE("Caught system exception COMM_FAILURE -- unable to contact the "
             << "naming service.");
    return 0;
  }
  catch(CORBA::SystemException&) {
    DEBTRACE("Caught a CORBA::SystemException while using the naming service.");
    return 0;
  }

  return 1;
}

static ostream& operator<<(ostream& os, const CORBA::Exception& e)
{
  CORBA::Any tmp;
  tmp<<= e;
  CORBA::TypeCode_var tc = tmp.type();
  const char *p = tc->name();
  if ( *p != '\0' ) {
    os<<p;
  }
  else  {
    os << tc->id();
  }
  return os;
}
