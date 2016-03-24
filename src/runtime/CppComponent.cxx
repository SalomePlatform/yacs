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

#include "RuntimeSALOME.hxx"
#include "CppComponent.hxx"
#include "CppContainer.hxx"
#include "TypeCode.hxx"
#include "CppNode.hxx"

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
         case NONE :
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

std::string CppComponent::getKindForNode() const
{
  return CppComponent::KIND;
}

//! CppComponent constructor
CppComponent::CppComponent(const std::string &name) : ComponentInstance(name), __obj(0), __run(0),__terminate(0)
{
  _container = getRuntime()->createContainer(CppNode::KIND);
  /* This part of code has been commented because the load part is supposed to do that !
  if (!_container->isAlreadyStarted(0))
    _container->start(0);
  CppContainer * _containerC = dynamic_cast<CppContainer *> (_container);
  _containerC->createInternalInstance(name, __obj, __run, __terminate);*/
}

//! CppComponent copy constructor
CppComponent::CppComponent(const CppComponent& other) : ComponentInstance(other._compoName), __obj(0), __run(0),__terminate(0)
{
  _container = getRuntime()->createContainer(CppNode::KIND);
  /* This part of code has been commented because the load part is supposed to do that !
  if (!_container->isAlreadyStarted(0))
    _container->start(0);
  CppContainer * _containerC = dynamic_cast<CppContainer *> (_container);  
   _containerC->createInternalInstance(_compoName, __obj, __run, __terminate);*/
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
  sDebug << getInstanceName() << "::" << service << "(";
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
void CppComponent::unload(Task *askingNode)
{
  //Not implemented
  DEBTRACE("CppComponent::unload : not implemented ");
}

//! Is the component instance already loaded ?
bool CppComponent::isLoaded(Task *askingNode) const
{
  return NULL != __obj;
}
 
void CppComponent::load(Task *askingNode)
{
  if (!_container)
    {
      _container = getRuntime()->createContainer(CppNode::KIND);
    }

  if(_container)
    {
      CppContainer *containerC(dynamic_cast< CppContainer *> (_container));
      if(!containerC)
        throw Exception("The type of container should be CPP for component CPP !");
      containerC->lock();//To be sure
      if(!_container->isAlreadyStarted(askingNode))
        {
          try
          {
              _container->start(askingNode);
          }
          catch(Exception& e)
          {
              containerC->unLock();
              throw e;
          }
        }
      containerC->unLock();
      containerC->lock();//To be sure

      bool isLoadable(containerC->loadComponentLibrary(_compoName));
      if (isLoadable) 
        containerC->createInternalInstance(_compoName, __obj, __run, __terminate);

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

YACS::ENGINE::ComponentInstance *CppComponent::cloneAlways() const
{
  return new CppComponent(*this);
}
