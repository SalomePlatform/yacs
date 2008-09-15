#ifndef __CONDITIONINPUTPORT_HXX__
#define __CONDITIONINPUTPORT_HXX__

#include "InputPort.hxx"

namespace YACS
{
  namespace ENGINE
  {
    class WhileLoop;

    class ConditionInputPort : public InputPort // public inheritance for correct dynamic cast from Port to ConditionInputPort in GUI part
    {
      friend class WhileLoop;
      OutPort *_outOfScopeBackLink;

      ConditionInputPort(const std::string& name, WhileLoop *node);
      ConditionInputPort(const ConditionInputPort& other, Node *newHelder);
      ~ConditionInputPort();
      void exSaveInit();
      void exRestoreInit();
      InputPort *clone(Node *newHelder) const;
      bool isLinkedOutOfScope() const;
      void edNotifyReferencedBy(OutPort *fromPort);
      void edNotifyDereferencedBy(OutPort *fromPort);
      void *get() const;
      void put(const void *data) throw(ConversionException);
      std::string dump();
    protected:
      Any *_value;
    public:
      bool getValue() const { return (_value ? _value->getBoolValue() : false); }
      virtual std::string typeName() {return "YACS__ENGINE__ConditionInputPort";}
    };
  }
}

#endif
