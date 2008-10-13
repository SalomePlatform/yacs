#include "Container.hxx"
#include "ComponentInstance.hxx"

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;
using namespace YACS::ENGINE;

Container::Container():_isAttachedOnCloning(false)
{
}

Container::~Container()
{
}

/*!
 * By calling this method the current container 'this' is not destined to be deeply copied on clone call.
 */
void Container::attachOnCloning() const
{
  _isAttachedOnCloning=true;
}

/*!
 * By calling this method the current container 'this' will be deeply copied on clone call.
 */
void Container::dettachOnCloning() const
{
  _isAttachedOnCloning=false;
}

bool Container::isAttachedOnCloning() const
{
  return _isAttachedOnCloning;
}

/*!
 * This method informs about the capability of the container to deal with CT an unpredictably number of components.
 * By default : True
 */
bool Container::isSupportingRTODefNbOfComp() const
{
  return true;
}

void Container::setProperty(const std::string& name, const std::string& value)
{
  DEBTRACE("Container::setProperty " << name << " " << value);
  _propertyMap[name]=value;
}

std::string Container::getProperty(const std::string& name)
{
  DEBTRACE("Container::getProperty " << name );
  return _propertyMap[name];
}

void Container::setProperties(std::map<std::string,std::string> properties)
{
  _propertyMap.clear();
  std::map<std::string,std::string>::iterator it;
  for (it = properties.begin(); it != properties.end(); ++it)
    {
      setProperty((*it).first, (*it).second); // setProperty virtual and derived
    }
}
