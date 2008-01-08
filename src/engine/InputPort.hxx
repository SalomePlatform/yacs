#ifndef __INPUTPORT_HXX__
#define __INPUTPORT_HXX__

#include "Any.hxx"
#include "InPort.hxx"
#include "Runtime.hxx"
#include "TypeCode.hxx"
#include "DataFlowPort.hxx"
#include "ConversionException.hxx"

#include <string>

namespace YACS
{
  namespace ENGINE
  {
    class OutPort;
/*! \brief Base class for Input Ports
 *
 * \ingroup Ports
 *
 */
    class InputPort : public DataFlowPort, public InPort
    {
      friend class Runtime; // for port creation
      friend class OutPort;
    public:
      static const char NAME[];
    public:
      virtual ~InputPort();

      std::string getNameOfTypeOfCurrentInstance() const;
      //! returns the final physical port behind 'this'.
      virtual InputPort *getPublicRepresentant() { return this; }
      virtual bool isIntermediate() const { return false; }
      virtual bool edIsManuallyInitialized() const;
      //!soon deprecated
      bool edIsInitialized() const;

      template<class T>
      void edInit(T value);
      void edInit(Any *value);
      void edInit(const std::string& impl,const void* value);
      virtual void edRemoveManInit();
      void checkBasicConsistency() const throw(Exception);
      virtual void exInit(bool start);
      virtual void exSaveInit() = 0;
      virtual void exRestoreInit() = 0;
      virtual InputPort *clone(Node *newHelder) const = 0;
      virtual bool isEmpty();

      virtual void *get() const = 0;
      virtual void put(const void *data) throw(ConversionException) = 0;
      virtual std::string dump();
      virtual void setStringRef(std::string strRef);
      virtual std::string typeName() {return "YACS__ENGINE__InputPort";}
    protected:
      InputPort(const InputPort& other, Node *newHelder);
      InputPort(const std::string& name, Node *node, TypeCode* type);
    protected:
      Any *_initValue;
      std::string _stringRef;
    };

/*! \brief Base class for Proxy Input Ports
 *
 * \ingroup Ports
 *
 */
    class ProxyPort : public InputPort
    {
    public:
      ProxyPort(InputPort* p);
      ~ProxyPort();
      
      void edRemoveAllLinksLinkedWithMe() throw(Exception);
      InputPort *clone(Node *newHelder) const;
      void edNotifyReferencedBy(OutPort *fromPort);
      void edNotifyDereferencedBy(OutPort *fromPort);
      std::set<OutPort *> edSetOutPort() const;
      InputPort *getPublicRepresentant();
      void *get() const;
      virtual void put(const void *data) throw(ConversionException) ;
      int edGetNumberOfLinks() const;
      bool isIntermediate() const { return true; }
      void exRestoreInit();
      void exSaveInit();
      void getAllRepresentants(std::set<InPort *>& repr) const;
      virtual std::string typeName() {return "YACS__ENGINE__ProxyPort";}
    protected:
      InputPort* _port;
    };

    template<class T>
    void InputPort::edInit(T value)
    { 
      InputPort *manuallySet=getRuntime()->adapt(this,
                                                 Runtime::RUNTIME_ENGINE_INTERACTION_IMPL_NAME,_type);
      Any* any=AtomAny::New(value);
      manuallySet->put((const void *) any);
      if(manuallySet!=this)
        delete manuallySet;
      any->decrRef();
      exSaveInit();
    }
  }
}

#endif
