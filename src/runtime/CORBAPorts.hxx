#ifndef _CORBAPORTS_HXX_
#define _CORBAPORTS_HXX_

#include <omniORB4/CORBA.h>

#include "InputPort.hxx"
#include "OutputPort.hxx"

namespace YACS
{
  namespace ENGINE
  {

    class InputCorbaPort : public InputPort
    {
    public:
      InputCorbaPort(const std::string& name, Node *node, TypeCode * type);
      virtual void put(const void *data) throw(ConversionException);
      void put(CORBA::Any *data) throw (ConversionException);
      virtual CORBA::Any * getAny();
    protected:
      CORBA::Any  _data;
      CORBA::ORB_ptr _orb;
    };

    class OutputCorbaPort : public OutputPort
    {
    public:
      OutputCorbaPort(const std::string& name, Node *node, TypeCode * type);
      virtual void put(const void *data) throw(ConversionException);
      void put(CORBA::Any *data) throw (ConversionException);
      virtual CORBA::Any * getAny();
      virtual CORBA::Any * getAnyOut();
      friend std::ostream & operator<< ( std::ostream &os,
					 const OutputCorbaPort& p);
    protected:
      CORBA::Any  _data;
      CORBA::ORB_ptr _orb;
    };


  }
}

#endif
