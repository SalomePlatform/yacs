#ifndef __COMPONENTDEFINITION_HXX__
#define __COMPONENTDEFINITION_HXX__

#include <map>
#include <string>

namespace YACS
{
  namespace ENGINE
  {
    class ServiceNode;

/*! \brief Base class for component definition.
 *
 *
 */
    class ComponentDefinition 
    {
    public:
      ComponentDefinition(const std::string& name);
      const std::string& getName() const { return _name; }
      virtual ~ComponentDefinition();
      std::map<std::string,ServiceNode*> _serviceMap;
    protected:
      std::string _name;
    };
  }
}

#endif
