#include "ComponentInstance.hxx"
#include "Container.hxx"

#include <iostream>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace YACS::ENGINE;
using namespace std;

const char ComponentInstance::KIND[]="";

const char ComponentInstance::NULL_FILE_REPR[]="No repr specified for ComponentInstance";

void ComponentInstance::setContainer(Container *cont)
{
  if (cont == _container) return;
  
  if(cont)
    cont->checkCapabilityToDealWith(this);
  if(_container)
    _container->decrRef();
  _container=cont;
  if(_container)
    _container->incrRef();
}

ComponentInstance::ComponentInstance(const std::string& name):_name(name),_isAttachedOnCloning(false),_container(0)
{
}

ComponentInstance::ComponentInstance(const ComponentInstance& other):_name(other._name),
                                                                     _container(0),
                                                                     _isAttachedOnCloning(other._isAttachedOnCloning)
{
  if(other._container)
    _container=other._container->clone();
}

ComponentInstance::~ComponentInstance()
{
  if(_container)
    _container->decrRef();
}

/*!
 * By calling this method the current container 'this' is not destined to be deeply copied on clone call.
 */
void ComponentInstance::attachOnCloning() const
{
  _isAttachedOnCloning=true;
}

std::string ComponentInstance::getFileRepr() const
{
  return NULL_FILE_REPR;
}

/*!
 * By calling this method the current container 'this' will be deeply copied on clone call.
 */
void ComponentInstance::dettachOnCloning() const
{
  _isAttachedOnCloning=false;
}

bool ComponentInstance::isAttachedOnCloning() const
{
  return _isAttachedOnCloning;
}

string ComponentInstance::getKind() const
{
  return KIND;
}
