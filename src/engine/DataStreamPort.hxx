#ifndef __DATASTREAMPORT_HXX__
#define __DATASTREAMPORT_HXX__

#include "Port.hxx"
#include "define.hxx"

#include <string>

namespace YACS
{
  namespace ENGINE
  {
    class DataStreamPort : public virtual Port
    {
    protected:
      std::string _name;
      StreamType _edType;
    public:
      static const char NAME[];
    protected:
      DataStreamPort(const std::string& name, Node *node, StreamType type);
    public:
      std::string getNameOfTypeOfCurrentInstance() const;
      std::string getName() const { return _name; }
      StreamType edGetType() const { return _edType; }
    };
  }
}

#endif
