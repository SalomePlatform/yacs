#ifndef __INPUTPORT_HXX__
#define __INPUTPORT_HXX__

#include "InPort.hxx"
#include "DataFlowPort.hxx"
#include "ConversionException.hxx"

namespace YACS
{
  namespace ENGINE
  {
    class InputPort : public DataFlowPort, public InPort
    {
    protected:
      bool _manuallySet;
    public:
      static const char NAME[];
    public:
      InputPort(const std::string& name, Node *node, DynType type);
      std::string getNameOfTypeOfCurrentInstance() const;
      void edInit(Data data) throw(ConversionException);
      void edNotifyReferenced();
      void exInit();
      Data exGet() const;
      void exAccept(Data data) throw(ConversionException);
      ~InputPort();
    };
  }
}

#endif
