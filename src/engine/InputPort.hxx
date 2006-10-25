#ifndef __INPUTPORT_HXX__
#define __INPUTPORT_HXX__

//#include <Python.h>
//#include <omniORB4/CORBA.h>

#include "TypeCode.hxx"
#include "InPort.hxx"
#include "DataFlowPort.hxx"
#include "ConversionException.hxx"

#include <string>

namespace YACS
{
  namespace ENGINE
  {

    class Runtime;

    class InputPort : public DataFlowPort, public InPort
    {
      friend class Runtime; // for port creation
    public:
      ~InputPort();

      std::string getNameOfTypeOfCurrentInstance() const;

      void edNotifyReferenced();
      void edInit(const void *data) throw(ConversionException);

      void exInit();
      bool isEmpty();

      virtual void put(const void *data) throw(ConversionException);

      static const char NAME[];

    protected:
      InputPort(const std::string& name, Node *node, TypeCode* type);
      bool _empty;
      bool _manuallySet;
    };


    class ProxyPort : public InputPort
    {
    public:
      ProxyPort(InputPort* p)
	: InputPort("Convertor", p->getNode(), p->type()),
	  Port( p->getNode())
      { _port = p; }
    protected:
      InputPort* _port;
    };



  }
}

#endif
