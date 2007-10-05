//To trace CORBA ref count, uncomment the following line 
//#define REFCNT
#ifdef REFCNT
#define private public
#include <omniORB4/CORBA.h>
#endif

#include "RuntimeSALOME.hxx"
#include "CORBAComponent.hxx"
#include "CORBANode.hxx"

#include <sstream>
#include <iostream>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace YACS::ENGINE;
using namespace std;

const char CORBAComponent::KIND[]="CORBA";

//! CORBAComponent constructor
CORBAComponent::CORBAComponent(const std::string& name): ComponentInstance(name)
{
  _objComponent=CORBA::Object::_nil();
}

//! CORBAComponent copy constructor
CORBAComponent::CORBAComponent(const CORBAComponent& other):ComponentInstance(other)
{
  _objComponent=CORBA::Object::_nil();
}

CORBAComponent::~CORBAComponent()
{
#ifdef REFCNT
  DEBTRACE( "+++++++++++++++++" << getName() << " +++++++++++++++++" );
  if(_objComponent != CORBA::Object::_nil())
    {
      std::cerr << "CORBAComponent::destructor:refcount: " <<_objComponent->_PR_getobj()->pd_refCount << std::endl;
    }
#endif
}

std::string CORBAComponent::getKind() const
{
  return KIND;
}

//! Unload the component 
void CORBAComponent::unload()
{
  //Not implemented
  std::cerr << "CORBAComponent::unload : not implemented " << std::endl;
}

CORBA::Object_ptr CORBAComponent::getCompoPtr()
{
#ifdef REFCNT
  std::cerr << "CORBAComponent::getCompoPtr:refCount: " <<_objComponent->_PR_getobj()->pd_refCount << std::endl;
#endif
  return CORBA::Object::_duplicate(_objComponent);
}

//! Is the component instance already loaded ?
bool CORBAComponent::isLoaded()
{
  if(CORBA::is_nil(_objComponent))
    return false;
  else
    return true;
}

//! Load the component 
void CORBAComponent::load()
{
  DEBTRACE( "CORBAComponent::load" );
  CORBA::ORB_ptr orb;
  try
    {
      DEBTRACE( "+++++++++++++++++" << getName() << " +++++++++++++++++" );
      orb = getSALOMERuntime()->getOrb();
      _objComponent= orb->string_to_object(getName().c_str());
#ifdef REFCNT
      std::cerr << "CORBAComponent::load:refCount: " <<_objComponent->_PR_getobj()->pd_refCount << std::endl;
#endif
    }
  catch(CORBA::COMM_FAILURE& ex) 
    {
      cerr << "Caught system exception COMM_FAILURE -- unable to contact the "
           << "object." << endl;
      throw Exception("Execution problem");
    }
  catch(CORBA::SystemException& ex) 
    {
      cerr << "Caught a CORBA::SystemException." ;
      CORBA::Any tmp;
      tmp <<= ex;
      CORBA::TypeCode_var tc = tmp.type();
      const char *p = tc->name();
      if ( *p != '\0' ) 
        cerr <<p;
      else  
        cerr  << tc->id();
      cerr << endl;
      throw Exception("Execution problem");
    }
  catch(CORBA::Exception& ex) 
    {
      cerr << "Caught CORBA::Exception. " ;
      CORBA::Any tmp;
      tmp <<= ex;
      CORBA::TypeCode_var tc = tmp.type();
      const char *p = tc->name();
      if ( *p != '\0' )
        cerr <<p;
      else 
        cerr  << tc->id();
      cerr << endl;
      throw Exception("Execution problem");
    }
  catch(omniORB::fatalException& fe) 
    {
      cerr << "Caught omniORB::fatalException:" << endl;
      cerr << "  file: " << fe.file() << endl;
      cerr << "  line: " << fe.line() << endl;
      cerr << "  mesg: " << fe.errmsg() << endl;
      throw Exception("Execution problem");
    }
  catch(...) 
    {
      cerr << "Caught unknown exception." << endl;
      throw Exception("Execution problem");
    }
  if( CORBA::is_nil(_objComponent) )
    {
      cerr << "Can't get reference to object (or it was nil)." << endl;
      throw Exception("Execution problem");
    }
  //TODO: if IOR is valid but the component does not exist, it works (bad)
}

//! Create a ServiceNode with this component instance and no input or output port
/*!
 *   \param name : node name
 *   \return       a new CORBANode node
 */
ServiceNode* CORBAComponent::createNode(const std::string& name)
{
   CORBANode* node=  new CORBANode(name);
   node->setComponent(this);
   return node;
}

//! Clone the component instance 
ComponentInstance* CORBAComponent::clone() const
{
  //no real need to clone a CORBA Component : there is no component instance loading
  incrRef();
  return (ComponentInstance*)this;
  //return new CORBAComponent(*this);
}

std::string CORBAComponent::getFileRepr() const
{
  ostringstream stream;
  stream << "<ref>" << getName() << "</ref>";
  return stream.str();
}
