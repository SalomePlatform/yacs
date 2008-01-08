#ifndef __PORT_HXX__
#define __PORT_HXX__

#include "TypeCode.hxx"
#include <string>

namespace YACS
{
  namespace ENGINE
  {
    class Node;

/*! \brief Base class for all ports
 *
 * Not instanciable class that factorizes all basic data and behaviours relative 
 * to the in and out interfaces of all nodes.
 * End-user should neither instanciate a sub-class of 'Port' 
 * nor call other methods than accessor.
 *
 * \ingroup Ports
 */
    class Port
    {
    public:
      virtual ~Port();
      Node *getNode() const { return _node; }
      virtual std::string getNameOfTypeOfCurrentInstance() const;
      int getNumId() const { return _id; }
      virtual std::string typeName() {return "YACS__ENGINE__Port";}
    protected:
      Port(Node *node);
      Port(const Port& other, Node *newHelder);
    protected:
      Node        *_node;
      int          _id;
      static int   _total;
      static const char NAME[];
    };
  }
}

#endif
