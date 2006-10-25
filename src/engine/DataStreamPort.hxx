#ifndef __DATASTREAMPORT_HXX__
#define __DATASTREAMPORT_HXX__

#include "Port.hxx"
#include "TypeCode.hxx"

#include <string>

namespace YACS
{
  namespace ENGINE
  {
    class DataStreamPort : public virtual Port
    {
    protected:
      std::string _name;
      TypeCode* _edType;
    public:
      static const char NAME[];
    protected:
      DataStreamPort(const std::string& name, Node *node, TypeCode* type);
    public:
      std::string getNameOfTypeOfCurrentInstance() const;
      std::string getName() const { return _name; }
      TypeCode* edGetType() const { return _edType; }
    };
  }
}

#endif
