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

#include <time.h>

#include <echo.hh>

#include <iostream>
using namespace std;

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

static CORBA::Boolean bindObjectToName(CORBA::ORB_ptr, CORBA::Object_ptr,const char*);

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

class Obj_i : public POA_eo::Obj, public PortableServer::RefCountServantBase
{
public:
  inline Obj_i() {}
  virtual ~Obj_i() {}
  CORBA::Long echoLong(CORBA::Long i);
};

class C_i : public POA_eo::C, public PortableServer::RefCountServantBase
{
public:
  inline C_i() {}
  virtual ~C_i() {}
  CORBA::Long echoLong(CORBA::Long i);
};

class D_i : public POA_eo::D, public PortableServer::RefCountServantBase
{
public:
  inline D_i() {}
  virtual ~D_i() {}
  CORBA::Long echoLong(CORBA::Long i);
  CORBA::Long echoLong2(CORBA::Long i);
};

class E_i : public POA_eo::E, public PortableServer::RefCountServantBase
{
public:
  inline E_i() {}
  virtual ~E_i() {}
  CORBA::Long echoLong(CORBA::Long i);
  CORBA::Long echoLong2(CORBA::Long i);
};

class Echo_i : public POA_eo::Echo,
               public PortableServer::RefCountServantBase
{
public:
  inline Echo_i() {}
  virtual ~Echo_i() {}
  virtual char* echoString(const char* mesg);
  CORBA::Long echoLong(CORBA::Long i) throw(eo::SALOME_Exception);
  void echoDouble(CORBA::Double i,CORBA::Double& j) ;
  void echoDoubleVec(const eo::DoubleVec& i,eo::DoubleVec_out j) ;
  void echoDoubleVecVec(const eo::DoubleVecVec&, eo::DoubleVecVec_out);
  void echoIntVec(const eo::IntVec&, eo::IntVec_out);
  void echoStrVec(const eo::StrVec&, eo::StrVec_out);
  void echoObjectVec(const eo::ObjectVec&, eo::ObjectVec_out);
  void echoObjVec(const eo::ObjVec&, eo::ObjVec_out);
  void echoObj2(eo::Obj_ptr , eo::Obj_out);
  void echoD(eo::D_ptr , eo::D_out);
  void echoC(eo::C_ptr , eo::C_out);
  void echoObjectVecVec(const eo::ObjectVecVec&, eo::ObjectVecVec_out);

  eo::Obj_ptr echoObj(CORBA::Long i, eo::Obj_ptr o, CORBA::Long j, eo::Obj_out oo);
  void createObj(CORBA::Long i, eo::Obj_out oo);
  void createC(eo::C_out oo);
  void echoAll(CORBA::Double d,CORBA::Long l,const char * m,eo::Obj_ptr o,CORBA::Double& dd,CORBA::Long& ll,CORBA::String_out s,eo::Obj_out oo);
  virtual PortableServer::POA_ptr _default_POA();
};

//Implementation Echo
PortableServer::POA_ptr Echo_i::_default_POA()
{
  PortableServer::POA_var root_poa=PortableServer::POA::_the_root_poa();
  try{
    return PortableServer::POA::_duplicate(root_poa->find_POA("shortcut",0));
  }
  catch(...){
    //return PortableServer::POA::_duplicate(root_poa);
    return root_poa._retn();
  }
}

char* Echo_i::echoString(const char* mesg)
{
  DEBTRACE("Echo_i::echoString " << mesg);
  return CORBA::string_dup(mesg);
}

void Echo_i::echoDouble(CORBA::Double i,CORBA::Double& j ) {
  DEBTRACE("Echo_i::echoDouble " << i);
  j=i+1;
}

void Echo_i::echoIntVec(const eo::IntVec& in, eo::IntVec_out out)
{
  DEBTRACE("Echo_i::echoIntVec " << in.length());
  for(int i=0;i<in.length(); i++){
    DEBTRACE(in[i]);
  };
  out=new eo::IntVec(in);
}

void Echo_i::echoStrVec(const eo::StrVec& in, eo::StrVec_out out)
{
  DEBTRACE("Echo_i::echoStrVec " << in.length());
  for(int i=0;i<in.length(); i++){
    DEBTRACE(in[i]);
  }
  out=new eo::StrVec(in);
}

void Echo_i::echoObjectVec(const eo::ObjectVec& in, eo::ObjectVec_out out)
{
  DEBTRACE("Echo_i::echoObjectVec " << in.length());
  for(int i=0;i<in.length(); i++){
    DEBTRACE(in[i]->_PD_repoId);
  };
  out=new eo::ObjectVec(in);
}

void Echo_i::echoObjVec(const eo::ObjVec& in, eo::ObjVec_out out)
  {
    DEBTRACE("Echo_i::echoObjVec " << in.length());
    for(int i=0;i<in.length(); i++)
      {
        DEBTRACE(in[i]->_mostDerivedRepoId());
      };

    out=new eo::ObjVec;
    out->length(2);

    Obj_i* obj0 = new Obj_i();
    CORBA::Object_var ref0 = obj0->_this();
    eo::Obj_var o0 = eo::Obj::_narrow(ref0);
    (*out)[0]=o0;
    obj0->_remove_ref();

    C_i* obj1 = new C_i();
    CORBA::Object_var ref1 = obj1->_this();
    eo::Obj_var o1 = eo::Obj::_narrow(ref1);
    (*out)[1]=o1;
    obj1->_remove_ref();

    DEBTRACE("ENDOF Echo_i::echoObjVec " );
}

void Echo_i::echoObjectVecVec(const eo::ObjectVecVec& in, eo::ObjectVecVec_out out)
{
  DEBTRACE("Echo_i::echoObjectVecVec " << in.length());
  for(int i=0;i< in.length(); i++){
    for(int j=0;j< in[i].length(); j++){
      DEBTRACE(in[i][j]->_PD_repoId);
    };
  };
  out=new eo::ObjectVecVec(in);
}

void Echo_i::echoDoubleVec(const eo::DoubleVec& in,eo::DoubleVec_out out ) 
{
  DEBTRACE("Echo_i::echoDoubleVec " << in.length());
  for(int i=0;i<in.length(); i++){
    DEBTRACE(in[i]);
  };
  out=new eo::DoubleVec(in);
}

void Echo_i::echoDoubleVecVec(const eo::DoubleVecVec& in, eo::DoubleVecVec_out out)
{
  DEBTRACE("Echo_i::echoDoubleVecVec " << in.length());
  for(int i=0;i< in.length(); i++){
    for(int j=0;j< in[i].length(); j++){
      DEBTRACE(in[i][j]);
    };
  };
  out=new eo::DoubleVecVec(in);
}

CORBA::Long Echo_i::echoLong(CORBA::Long i ) throw(eo::SALOME_Exception) 
{
  DEBTRACE("Echo_i::echoLong " << i);
  if(i < 0) {
    eo::ExceptionStruct es;
    es.type = eo::COMM;
    es.text = "error Socket exception";
    throw eo::SALOME_Exception(es);
  }

  CORBA::Long j=i+1;
  return j;
}

void Echo_i::echoC(eo::C_ptr o,eo::C_out oo){
  DEBTRACE("Echo_i::echoC ");
  o->echoLong(10);
  oo=eo::C::_duplicate(o); 
}

void Echo_i::echoD(eo::D_ptr o,eo::D_out oo){
  DEBTRACE("Echo_i::echoD ");
  o->echoLong2(10);
  //oo=eo::D::_duplicate(o); 
  D_i* myD = new D_i();
  oo=myD->_this();
  myD->_remove_ref();
}

void Echo_i::echoObj2(eo::Obj_ptr o,eo::Obj_out oo){
  DEBTRACE("Echo_i::echoObj2 ");
  o->echoLong(10);
  oo=eo::Obj::_duplicate(o); 
}

eo::Obj_ptr Echo_i::echoObj(CORBA::Long i ,eo::Obj_ptr o,CORBA::Long j,eo::Obj_out oo){
  DEBTRACE("Echo_i::echoObj " << i << "," << j );
  oo=eo::Obj::_duplicate(o); 
  return eo::Obj::_duplicate(o); 
}

void Echo_i::createObj(CORBA::Long i ,eo::Obj_out oo){
  DEBTRACE("Echo_i::createObj " << i);
  Obj_i* myobj = new Obj_i();
  CORBA::Object_var myref = myobj->_this();
  oo = eo::Obj::_narrow(myref);
  myobj->_remove_ref();
}

void Echo_i::createC(eo::C_out oo){
  DEBTRACE("Echo_i::createC ");
  C_i* myobj = new C_i();
  CORBA::Object_var myref = myobj->_this();
  oo = eo::C::_narrow(myref);
  myobj->_remove_ref();
}

void Echo_i::echoAll(CORBA::Double d,CORBA::Long l,const char * m,eo::Obj_ptr o,CORBA::Double& dd,CORBA::Long& ll,CORBA::String_out s,eo::Obj_out oo)
{
  DEBTRACE("Echo_i::echoAll " << d << "," << l << "," << m);
  dd=d;
  ll=l;
  s=CORBA::string_dup(m);
  oo=eo::Obj::_duplicate(o); 
};

//Implementation Obj
CORBA::Long Obj_i::echoLong(CORBA::Long i ){
  DEBTRACE("Obj_i::echoLong " << i );
  CORBA::Long j=i+1;
  return j;
}

//Implementation C
CORBA::Long C_i::echoLong(CORBA::Long i ){
  DEBTRACE("C_i::echoLong " << i);
  CORBA::Long j=i+5;
  return j;
}

//Implementation D
CORBA::Long D_i::echoLong2(CORBA::Long i ){
  DEBTRACE("D_i::echoLong " << i);
  CORBA::Long j=i+10;
  return j;
}
CORBA::Long D_i::echoLong(CORBA::Long i ){
  DEBTRACE("D_i::echoLong " << i);
  CORBA::Long j=i+1;
  return j;
}

//Implementation E
CORBA::Long E_i::echoLong2(CORBA::Long i ){
  DEBTRACE("E_i::echoLong " << i);
  CORBA::Long j=i+20;
  return j;
}
CORBA::Long E_i::echoLong(CORBA::Long i ){
  DEBTRACE("E_i::echoLong " << i);
  CORBA::Long j=i+15;
  return j;
}

CORBA::ORB_ptr orb;
eo::Echo_var myechoref;

int main(int argc, char** argv)
{
  try {
    orb = CORBA::ORB_init(argc, argv);

    {
      CORBA::Object_var obj = orb->resolve_initial_references("RootPOA");
      PortableServer::POA_var root_poa = PortableServer::POA::_narrow(obj);
      // POA manager
      PortableServer::POAManager_var poa_man = root_poa->the_POAManager();
      poa_man->activate();

      // Create a new POA with the shortcut policy
      CORBA::PolicyList pl2;
      pl2.length(2);
      CORBA::Any v;
      v <<= omniPolicy::LOCAL_CALLS_SHORTCUT;
      pl2[0] = orb->create_policy(omniPolicy::LOCAL_SHORTCUT_POLICY_TYPE, v);
      pl2[1] = root_poa->create_implicit_activation_policy(PortableServer::IMPLICIT_ACTIVATION);
      PortableServer::POA_ptr shortcut_poa = root_poa->create_POA("shortcut", poa_man, pl2);

      // Create and activate servant
      Echo_i* myecho = new Echo_i();
      // Obtain a reference to the object, and print it out as a
      // stringified IOR.
      obj = myecho->_this();
      CORBA::String_var sior(orb->object_to_string(obj));
      DEBTRACE("'" << (char*)sior << "'");
      myechoref = eo::Echo::_narrow(obj);

      if( !bindObjectToName(orb, myechoref,"Echo") ) return 1;

      // Decrement the reference count of the object implementation, so
      // that it will be properly cleaned up when the POA has determined
      // that it is no longer needed.
      myecho->_remove_ref();

      //create object C and register it in naming service
      C_i* myC = new C_i();
      obj=myC->_this();
      eo::C_var myCref=eo::C::_narrow(obj);
      myC->_remove_ref();
      if( !bindObjectToName(orb, myCref,"C") ) return 1;

      //create object D and register it in naming service
      D_i* myD = new D_i();
      obj=myD->_this();
      eo::D_var myDref=eo::D::_narrow(obj);
      myD->_remove_ref();
      if( !bindObjectToName(orb, myDref,"D") ) return 1;

      //create object Obj and register it in naming service
      Obj_i* myObj = new Obj_i();
      obj=myObj->_this();
      eo::Obj_var myObjref=eo::Obj::_narrow(obj);
      myObj->_remove_ref();
      if( !bindObjectToName(orb, myObjref,"Obj") ) return 1;
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

    // Bind objref with name Echo to the testContext:
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
    // Note: Using rebind() will overwrite any Object previously bound
    //       to /test/Echo with obj.
    //       Alternatively, bind() can be used, which will raise a
    //       CosNaming::NamingContext::AlreadyBound exception if the name
    //       supplied is already bound to an object.

    // Amendment: When using OrbixNames, it is necessary to first try bind
    // and then rebind, as rebind on it's own will throw a NotFoundexception if
    // the Name has not already been bound. [This is incorrect behaviour -
    // it should just bind].
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

