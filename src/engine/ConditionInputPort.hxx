#ifndef __CONDITIONINPUTPORT_HXX__
#define __CONDITIONINPUTPORT_HXX__

#include "InputPort.hxx"

namespace YACS
{
  namespace ENGINE
  {
    class WhileLoop;

    class ConditionInputPort : public InputPort 
    {
      friend class WhileLoop;
    private:
      OutPort *_outOfScopeBackLink;
      ConditionInputPort(const std::string& name, WhileLoop *node);
      ConditionInputPort(const ConditionInputPort& other, Node *newHelder);
      virtual ~ConditionInputPort();
      InputPort *clone(Node *newHelder) const;
    public:
      void exSaveInit();
      void exRestoreInit();
      bool isLinkedOutOfScope() const;
      void edNotifyReferencedBy(OutPort *fromPort);
      void edNotifyDereferencedBy(OutPort *fromPort);
      void *get() const;
      virtual void put(const void *data) throw(ConversionException);
      void put(Any *data) throw(ConversionException);
      std::string dump();
      virtual std::string getAsString();
    protected:
      Any *_value;
    public:
      bool getValue() const { return (_value ? _value->getBoolValue() : false); }
      virtual std::string typeName() {return "YACS__ENGINE__ConditionInputPort";}
    };
  }
}

#endif
