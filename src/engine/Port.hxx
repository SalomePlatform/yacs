#ifndef __PORT_HXX__
#define __PORT_HXX__

#include "TypeCode.hxx"
#include <string>


/**
 * Not instanciable class that factorizes all basic data and behaviours relative 
 * to the in and out interfaces of all nodes.
 * End-user should neither instanciate a sub-class of 'Port' 
 * nore call other methods than accessor.
 */

namespace YACS
{
  namespace ENGINE
  {
    class Node;

    class Port
    {
    public:
      virtual ~Port();

      Node *getNode() const { return _node; }
      std::string getImpl() const {return _impl; }

      virtual std::string getNameOfTypeOfCurrentInstance() const;
      //      virtual TypeCode * type();

      static const char NAME[];

    protected:
      Port(Node *node);

      Node        *_node;
      std::string  _impl;
      TypeCode    *_type;
      int          id_;
      static int   total_;
    };
  }
}

#endif
