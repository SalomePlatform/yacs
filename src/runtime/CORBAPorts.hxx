#ifndef _CORBAPORTS_HXX_
#define _CORBAPORTS_HXX_

#include <omniORB4/CORBA.h>

#include "InputPort.hxx"
#include "OutputPort.hxx"
#include <string>

namespace YACS
{
  namespace ENGINE
  {
/*! \brief Class for CORBA Input Ports
 *
 * \ingroup Ports
 *
 * \see CORBANode
 */
    class InputCorbaPort : public InputPort
    {
    public:
      InputCorbaPort(const std::string& name, Node *node, TypeCode * type);
      InputCorbaPort(const InputCorbaPort& other, Node *newHelder);
      virtual ~InputCorbaPort();
      bool edIsManuallyInitialized() const;
      void edRemoveManInit();
      virtual void put(const void *data) throw(ConversionException);
      void put(CORBA::Any *data) throw (ConversionException);
      InputPort *clone(Node *newHelder) const;
      void *get() const throw(Exception);
      virtual bool isEmpty();
      virtual CORBA::Any * getAny();
      virtual void exSaveInit();
      virtual void exRestoreInit();
      virtual std::string dump();
    protected:
      CORBA::Any  _data;
      CORBA::Any *  _initData;
      CORBA::ORB_ptr _orb;
    };

    class OutputCorbaPort : public OutputPort
    {
    public:
      OutputCorbaPort(const std::string& name, Node *node, TypeCode * type);
      OutputCorbaPort(const OutputCorbaPort& other, Node *newHelder);
      virtual ~OutputCorbaPort();
      virtual void put(const void *data) throw(ConversionException);
      void put(CORBA::Any *data) throw (ConversionException);
      OutputPort *clone(Node *newHelder) const;
      virtual CORBA::Any * getAny();
      virtual CORBA::Any * getAnyOut();
      virtual std::string dump();
      friend std::ostream & operator<< ( std::ostream &os,
                                         const OutputCorbaPort& p);
    protected:
      CORBA::Any  _data;
      CORBA::ORB_ptr _orb;
    };


  }
}

#endif
