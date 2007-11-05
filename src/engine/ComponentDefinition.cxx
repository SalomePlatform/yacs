#include "ComponentDefinition.hxx"
#include "ServiceNode.hxx"

#include <iostream>

#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace YACS::ENGINE;
using namespace std;

ComponentDefinition::ComponentDefinition(const std::string& name):_name(name)
{
}

ComponentDefinition::~ComponentDefinition()
{
  DEBTRACE("ComponentDefinition::~ComponentDefinition");
  //get rid of service nodes in  map
  std::map<std::string, ServiceNode*>::const_iterator lt;
  for(lt=_serviceMap.begin();lt!=_serviceMap.end();lt++)
    delete (*lt).second;
}

