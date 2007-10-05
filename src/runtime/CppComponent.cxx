#include "RuntimeSALOME.hxx"
#include "CppComponent.hxx"
#include "CppContainer.hxx"
#include "TypeCode.hxx"
#include "CppNode.hxx"
#include "DynLibLoader.hxx"

using namespace YACS::ENGINE;
 
#include <iostream>
#include <sstream>
#include <cstdlib>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

const char CppComponent::KIND[] = "Cpp";

static std::ostream & operator<<(std::ostream & f, const Any & A)
{
    const TypeCode * t = A.getType();
    if (NULL == t)
       f << "(type NULL)";
    else 
       switch (t->kind()) {
         case None :
            f << "(type None)";
            break;
         case Double :
            f << "(type Double) " << A.getDoubleValue();
            break;
         case Int :
            f << "(type Int) " << A.getIntValue();
            break;
         case String :
            f << "(type String) " << A.getStringValue();
            break;
         case Bool :
            f << "(type Bool) " << A.getBoolValue();
            break;
         case Objref :
	    f << "(type Objref)";
            break;
         case Sequence :
	    f << "(type Sequence) ";
	    {
              int i;
	      const SequenceAny * sA = dynamic_cast<const SequenceAny *>(&A);
	      for (i=0; i<sA->size(); i++)
		      f << " " << *((*sA)[i]);
	    }
            break;
       }
    return f;
}

std::string CppComponent::getKind() const
{
   return CppComponent::KIND;
}

//! CppComponent constructor
CppComponent::CppComponent(const std::string &name) : ComponentInstance(name)
{
  _container = getRuntime()->createContainer(CppNode::KIND);
  if (!_container->isAlreadyStarted())
	  _container->start();
  
  CppContainer * _containerC = dynamic_cast<CppContainer *> (_container);
  _containerC->createInternalInstance(name, __obj, __run, __terminate);
}

//! CppComponent copy constructor
CppComponent::CppComponent(const CppComponent& other) : ComponentInstance(other._name), __run(other.__run),
                                                        __terminate(other.__terminate), __obj(0)
{
  _container = getRuntime()->createContainer(CppNode::KIND);
  if (!_container->isAlreadyStarted())
    _container->start();

  CppContainer * _containerC = dynamic_cast<CppContainer *> (_container);  
   _containerC->createInternalInstance(_name, __obj, __run, __terminate);
}

CppComponent::~CppComponent()
{
    DEBTRACE("CppComponent::~CppComponent()");
    if (__terminate) __terminate(&__obj);
    if (_container)
    	((CppContainer *) _container)->unregisterComponentInstance(this);
}

void CppComponent::run (const char * service, int nbIn, int nbOut,
				                 Any ** argIn, Any ** argOut) throw (YACS::Exception)
{
  int i;
  returnInfo return_code;
    
#ifdef _DEVDEBUG_
  std::ostringstream sDebug;
  sDebug << _name << "::" << service << "(";
  for (i=0; i<nbIn; i++) {
     sDebug << *(argIn[i]);
     if (i<nbIn-1)
        sDebug << ", ";
  }
  sDebug << ")";
#endif

  __run(__obj, service, nbIn, nbOut, argIn, argOut, &return_code);
    
  if (return_code.code != 0) {
#ifdef _DEVDEBUG_
    std::cerr << sDebug << " ???" << std::endl;
#endif
    throw YACS::Exception(return_code.message);
  }

#ifdef _DEVDEBUG_
  if (nbOut > 0) {
    sDebug << "->";
    for (i=0; i<nbOut; i++) {
       sDebug << " " << *(argOut[i]);
    }
  }
  DEBTRACE(sDebug.str());
#endif
}

//! Unload the component 
void CppComponent::unload()
{
  //Not implemented
  DEBTRACE("CppComponent::unload : not implemented ");
}

//! Is the component instance already loaded ?
bool CppComponent::isLoaded()
{
  return NULL != __obj;
}
 
void CppComponent::load()
{
   if (!_container) {
	   _container = getRuntime()->createContainer(CppNode::KIND);
   }
   
   if(_container) {
		
      CppContainer * containerC= dynamic_cast< CppContainer *> (_container);
      
      containerC->lock();//To be sure
      if(!_container->isAlreadyStarted())
        {
          try
            {
              _container->start();
            }
          catch(Exception& e)
            {
              containerC->unLock();
              throw e;
            }
        }
      containerC->unLock();
      containerC->lock();//To be sure
      
      YACS::BASES::DynLibLoader D(_name + "Local");
      
      bool isLoadable = containerC->loadComponentLibrary(_name);
      if (isLoadable) 
        containerC->createInternalInstance(_name, __obj, __run, __terminate);
        
      if(NULL == __obj)
        {
          containerC->unLock();
          throw Exception("CppComponent::load : Error while trying to create a new component.");
        }
      containerC->unLock();
      return;
    }
	
}

ServiceNode* CppComponent::createNode(const std::string& name)
{
   CppNode* node=new CppNode(name);
   node->setComponent(this);
   return node;
}

//! Clone the component instance
ComponentInstance* CppComponent::clone() const
{
  if(_isAttachedOnCloning)
    {
      incrRef();
      return (ComponentInstance*) (this);
    }
  else
    return new CppComponent(*this);
}

