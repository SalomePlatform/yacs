#ifndef __DATASTREAMPORT_HXX__
#define __DATASTREAMPORT_HXX__

#include "DataPort.hxx"

#include <string>
#include <map>

namespace YACS
{
  namespace ENGINE
  {
    class DataStreamPort : public virtual DataPort
    {
    public:
      static const char NAME[];
    protected:
      DataStreamPort(const DataStreamPort& other, Node *newHelder);
      DataStreamPort(const std::string& name, Node *node, TypeCode* type);
      std::map<std::string,std::string> _propertyMap;
    public:
      std::string getNameOfTypeOfCurrentInstance() const;
      TypeOfChannel getTypeOfChannel() const { return DATASTREAM; }
      virtual void setProperty(const std::string& name,const std::string& value);
      virtual void setProperties(std::map<std::string,std::string> properties);
      virtual std::string getProperty(const std::string& name);
      std::map<std::string,std::string> getPropertyMap() const { return _propertyMap; }
      virtual void initPortProperties();
      virtual ~DataStreamPort();
      virtual std::string typeName() {return "YACS__ENGINE__DataStreamPort";}
    };
  }
}

#endif
