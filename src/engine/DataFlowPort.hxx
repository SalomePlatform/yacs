#ifndef __DATAFLOWPORT_HXX__
#define __DATAFLOWPORT_HXX__

#include "Port.hxx"

#include <string>

namespace YACS
{
  namespace ENGINE
  {
    class DataFlowPort : public virtual Port
    {
    protected:
      void* _data;
      std::string _name;
    public:
      static const char NAME[];
    protected:
      DataFlowPort(const std::string& name, Node *node, TypeCode* type);
    public:
      std::string getNameOfTypeOfCurrentInstance() const;
      TypeCode* edGetType() const;
      virtual TypeCode * type();
      virtual void edSetType(TypeCode* type);
      std::string getName() const { return _name; }
    };
  }
}

#endif
