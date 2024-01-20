// Copyright (C) 2006-2024  CEA, EDF
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
#include <pthread.h>
#ifdef WIN32
#include <windows.h>
#define sleep _sleep
#else
#include <unistd.h>
#endif

#include <echo.hh>

#include <iostream>
#include <memory>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

#include <SALOMEconfig.h>
#include CORBA_CLIENT_HEADER(SALOME_Embedded_NamingService)

using namespace std;

CORBA::ORB_var orb;
static CORBA::Boolean bindObjectToName(Engines::EmbeddedNamingService_ptr ns, CORBA::Object_ptr,const char*);

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
  virtual CORBA::Boolean echoBoolean(CORBA::Boolean b);
  CORBA::Long echoLong(CORBA::Long i);
  void echoDouble(CORBA::Double i,CORBA::Double& j) ;
  void echoDoubleVec(const eo::DoubleVec& i,eo::DoubleVec_out j) ;
  void echoDoubleVecVec(const eo::DoubleVecVec&, eo::DoubleVecVec_out);
  void echoIntVec(const eo::IntVec&, eo::IntVec_out);
  void echoStrVec(const eo::StrVec&, eo::StrVec_out);
  void echoBoolVec(const eo::BoolVec&, eo::BoolVec_out);
  void echoObjectVec(const eo::ObjectVec&, eo::ObjectVec_out);
  void echoObj2(eo::Obj_ptr , eo::Obj_out);
  void echoD(eo::D_ptr , eo::D_out);
  void echoC(eo::C_ptr , eo::C_out);
  void echoObjectVecVec(const eo::ObjectVecVec&, eo::ObjectVecVec_out);

  eo::Obj_ptr echoObj(CORBA::Long i, eo::Obj_ptr o, CORBA::Long j, eo::Obj_out oo);
  void createObj(CORBA::Long i, eo::Obj_out oo);
  void createC(eo::C_out oo);
  void echoAll(CORBA::Double d,CORBA::Long l,const char * m,eo::Obj_ptr o,CORBA::Double& dd,CORBA::Long& ll,CORBA::String_out s,eo::Obj_out oo);
  void sleepLong(CORBA::Double time1,CORBA::Double& time2) ;
  virtual eo::S2* echoStruct(const eo::S2&);
  virtual eo::S3* echoStruct2(const eo::S3&);
  virtual void shutdown();
  virtual PortableServer::POA_ptr _default_POA();
protected:
  int _ctr;
  pthread_mutex_t _mutex;
};

//Implementation Echo
PortableServer::POA_ptr Echo_i::_default_POA()
{
  _ctr =0;
  pthread_mutex_init(&_mutex, NULL);
  PortableServer::POA_var root_poa=PortableServer::POA::_the_root_poa();
  try{
    return PortableServer::POA::_duplicate(root_poa->find_POA("shortcut",0));
  }
  catch(...){
    //return PortableServer::POA::_duplicate(root_poa);
    return root_poa._retn();
  }
}


void Echo_i::shutdown()
{
  // Shutdown the ORB (but do not wait for completion).  This also
  // causes the main thread to unblock from CORBA::ORB::run().
  orb->shutdown(0);
}

char* Echo_i::echoString(const char* mesg)
{
  DEBTRACE("Echo_i::echoString " << mesg);
  return CORBA::string_dup(mesg);
}

CORBA::Boolean Echo_i::echoBoolean(CORBA::Boolean b ) 
{
  DEBTRACE("Echo_i::echoBoolean " << b);
  return b;
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

void Echo_i::echoBoolVec(const eo::BoolVec& in, eo::BoolVec_out out)
{
  DEBTRACE("Echo_i::echoBoolVec " << in.length());
  for(int i=0;i<in.length(); i++){
    DEBTRACE(in[i]);
  };
  out=new eo::BoolVec(in);
}

void Echo_i::echoObjectVec(const eo::ObjectVec& in, eo::ObjectVec_out out)
{
  DEBTRACE("Echo_i::echoObjectVec " << in.length());
  for(int i=0;i<in.length(); i++){
    DEBTRACE(in[i]->_PD_repoId);
  };
  out=new eo::ObjectVec(in);
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

CORBA::Long Echo_i::echoLong(CORBA::Long i )
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

void Echo_i::sleepLong(CORBA::Double time1, CORBA::Double& time2)
{
  DEBTRACE("Echo_i::sleepLong");
  pthread_mutex_lock(&_mutex);
  int num = _ctr++;
  pthread_mutex_unlock(&_mutex);
  DEBTRACE("Echo_i::sleepLong start " << num);
  unsigned int t=(unsigned int) time1;
  sleep(t);
  DEBTRACE("Echo_i::sleepLong stop  " << num);
  time2 = time1;
}

eo::S2* Echo_i::echoStruct(const eo::S2& s)
{
  DEBTRACE("Echo_i::echoStruct " << s.s.x << " " << s.s.y);
  eo::S1 s1;
  s1.x=10.;
  s1.y=2;
  eo::S2* s2=new eo::S2;
  s2->s=s1;
  return s2;
}

eo::S3* Echo_i::echoStruct2(const eo::S3& s)
{
  DEBTRACE("Echo_i::echoStruct " << s.x << " " << s.y);
  if( !CORBA::is_nil(s.ob) ) 
    {
      std::cerr << s.ob->echoLong(10) << std::endl;
    }
  eo::S3* s2=new eo::S3;
  s2->x=10.;
  s2->y=2;
  return s2;
}

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

eo::Echo_var myechoref;

int main(int argc, char** argv)
{
  try {
    orb = CORBA::ORB_init(argc, argv);

    std::string theIOR( argv[1] );
    
    CORBA::Object_var obj = orb->string_to_object(theIOR.c_str());
    Engines::EmbeddedNamingService_var ns = Engines::EmbeddedNamingService::_narrow( obj );
    if( CORBA::is_nil( ns ) )
      return 1;
    //std::cout << getpid() << std::endl;
    //usleep(20000000);

    {
      CORBA::Object_var obj = orb->resolve_initial_references("RootPOA");
      PortableServer::POA_var root_poa = PortableServer::POA::_narrow(obj);
      // POA manager
      PortableServer::POAManager_var poa_man = root_poa->the_POAManager();
      poa_man->activate();

      // Create a new POA with the shortcut policy
      /*
      CORBA::PolicyList pl2;
      pl2.length(2);
      CORBA::Any v;
      v <<= omniPolicy::LOCAL_CALLS_SHORTCUT;
      pl2[0] = orb->create_policy(omniPolicy::LOCAL_SHORTCUT_POLICY_TYPE, v);
      pl2[1] = root_poa->create_implicit_activation_policy(PortableServer::IMPLICIT_ACTIVATION);
      PortableServer::POA_var shortcut_poa = root_poa->create_POA("shortcut", poa_man, pl2);
      */

      // Create and activate servant
      Echo_i* myecho = new Echo_i();
      // Obtain a reference to the object
      CORBA::Object_var obj2 = myecho->_this();
      myechoref = eo::Echo::_narrow(obj2);
      // Decrement the reference count of the object implementation, so
      // that it will be properly cleaned up when the POA has determined
      // that it is no longer needed.
      myecho->_remove_ref();

      // print the reference as a stringified IOR.
      CORBA::String_var sior(orb->object_to_string(obj2));
      DEBTRACE("'" << (char*)sior << "'");
      if( !bindObjectToName(ns, myechoref,"Echo") ) return 1;

      //create object C and register it in naming service
      C_i* myC = new C_i();
      CORBA::Object_var obj3 =myC->_this();
      eo::C_var myCref=eo::C::_narrow(obj3);
      myC->_remove_ref();
      if( !bindObjectToName(ns, myCref,"C") ) return 1;

      //create object D and register it in naming service
      D_i* myD = new D_i();
      CORBA::Object_var obj4=myD->_this();
      eo::D_var myDref=eo::D::_narrow(obj4);
      myD->_remove_ref();
      if( !bindObjectToName(ns, myDref,"D") ) return 1;

      //create object Obj and register it in naming service
      Obj_i* myObj = new Obj_i();
      CORBA::Object_var obj5=myObj->_this();
      eo::Obj_var myObjref=eo::Obj::_narrow(obj5);
      myObj->_remove_ref();
      if( !bindObjectToName(ns, myObjref,"Obj") ) return 1;
    }
    orb->run();
    std::cout << "Returned from orb->run()." << std::endl;
    orb->destroy();
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
bindObjectToName(Engines::EmbeddedNamingService_ptr ns, CORBA::Object_ptr objref,const char *name)
{
  CORBA::String_var iorNSUg = orb->object_to_string(objref);
  std::string iorNS(iorNSUg);
  Engines::IORType iorInput;
  auto len = iorNS.length();
  iorInput.length( len );
  for(auto i = 0 ; i < len ; ++i)
    iorInput[i] = iorNS[i];
  ns->Register(iorInput,name);
  return 1;
}

