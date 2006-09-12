#ifndef __DATAFLOWPORT_HXX__
#define __DATAFLOWPORT_HXX__

#include "Port.hxx"
#include "Data.hxx"

#include <string>

namespace YACS
{
  namespace ENGINE
  {
    class DataFlowPort : public virtual Port
    {
    protected:
      Data _data;
      std::string _name;
    public:
      static const char NAME[];
    protected:
      DataFlowPort(const std::string& name, Node *node, DynType type);
    public:
      std::string getNameOfTypeOfCurrentInstance() const;
      DynType edGetType() const;
      virtual void edSetType(DynType type);
      std::string getName() const { return _name; }
    };
  }
}

#endif
