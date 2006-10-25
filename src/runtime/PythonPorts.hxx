
#ifndef _PYTHONPORTS_HXX_
#define _PYTHONPORTS_HXX_

#include <Python.h>

#include "InputPort.hxx"
#include "OutputPort.hxx"

namespace YACS
{
  namespace ENGINE
  {

    class InputPyPort : public InputPort
    {
    public:
      InputPyPort(const std::string& name, Node * node, TypeCode * type);
      virtual void put(const void *data) throw(ConversionException);
      void put(PyObject *data) throw(ConversionException);
      virtual PyObject * getPyObj() const;
    protected:
      PyObject* _data;
    };

    class OutputPyPort : public OutputPort
    {
    public:
      OutputPyPort(const std::string& name, Node * node, TypeCode * type);
      virtual void put(const void *data) throw(ConversionException);
      void put(PyObject *data) throw(ConversionException);
      virtual PyObject * get() const;
    protected:
      PyObject* _data;
    };



  }
}

#endif
