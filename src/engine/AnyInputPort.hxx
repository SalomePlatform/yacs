#ifndef __ANYINPUTPORT_HXX__
#define __ANYINPUTPORT_HXX__

#include "InputPort.hxx"
#include <string>

namespace YACS
{
  namespace ENGINE
  {
    class AnyInputPort : public InputPort
    {
    public:
      AnyInputPort(const std::string& name, Node *node, TypeCode* type);
      AnyInputPort(const  AnyInputPort& other, Node *newHelder);
      virtual ~AnyInputPort();
      void exSaveInit();
      void exRestoreInit();
      Any *getValue() const { return _value; }
      int getIntValue() const { return _value->getIntValue(); }
      void put(Any *data);
      void *get() const;
      virtual bool isEmpty();
      void put(const void *data) throw(ConversionException);
      InputPort *clone(Node *newHelder) const;
      std::string dump();
      virtual std::string typeName() {return "YACS__ENGINE__AnyInputPort";}
    protected:
      Any *_value;
    };
  }
}

#endif
