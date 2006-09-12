#ifndef __PORT_HXX__
#define __PORT_HXX__

#include <string>

namespace YACS
{
  namespace ENGINE
  {
    class Node;
  }
}

/**
 *
 * Not instanciable class that factorizes all basic data and behaviours relative the in and out interfaces of all nodes.
 * End-user should not neither instanciate a sub-class of 'Port' nor called other methods than accessor.
 *
 */
namespace YACS
{
  namespace ENGINE
  {
    class Port
    {
    protected:
      Node *_node;//NOT OWNERED
    public:
      static const char NAME[];
    protected:
      Port(Node *node);
    public:
      virtual ~Port();
      Node *getNode() const { return _node; }
      virtual std::string getNameOfTypeOfCurrentInstance() const;
    };
  }
}

#endif
