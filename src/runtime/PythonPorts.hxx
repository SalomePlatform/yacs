
#ifndef _PYTHONPORTS_HXX_
#define _PYTHONPORTS_HXX_

#include <Python.h>

#include "InputPort.hxx"
#include "OutputPort.hxx"

namespace YACS
{
  namespace ENGINE
  {
    class InterpreterUnlocker 
    {
    public:
      InterpreterUnlocker() {
          gstate_ = PyGILState_Ensure();
      }
      ~InterpreterUnlocker() {
          PyGILState_Release(gstate_);
      }
    private:
      PyGILState_STATE gstate_;
    };

    typedef PyObject PyObj;

/*! \brief Class for Python Ports
 *
 * \ingroup Ports
 *
 * \see PythonNode
 */
    class InputPyPort : public InputPort
    {
    public:
      InputPyPort(const std::string& name, Node * node, TypeCode * type);
      InputPyPort(const InputPyPort& other, Node *newHelder);
      ~InputPyPort();
      bool edIsManuallyInitialized() const;
      void edRemoveManInit();
      virtual void put(const void *data) throw(ConversionException);
      void put(PyObject *data) throw(ConversionException);
      InputPort *clone(Node *newHelder) const;
      //special typedef PyObj used in SWIG to increment ref count on output
      virtual PyObj * getPyObj() const;
      virtual std::string getAsString();
      void *get() const throw(Exception);
      virtual bool isEmpty();
      virtual void exSaveInit();
      virtual void exRestoreInit();
      virtual std::string dump();
      virtual std::string typeName() {return "YACS__ENGINE__InputPyPort";}
      virtual std::string valToStr();
      virtual void valFromStr(std::string valstr);

    protected:
      PyObject* _data;
      PyObject* _initData;
    };

    class OutputPyPort : public OutputPort
    {
    public:
      OutputPyPort(const std::string& name, Node * node, TypeCode * type);
      OutputPyPort(const OutputPyPort& other, Node *newHelder);
      ~OutputPyPort();
      virtual void put(const void *data) throw(ConversionException);
      void put(PyObject *data) throw(ConversionException);
      OutputPort *clone(Node *newHelder) const;
      virtual PyObject * get() const;
      //special typedef PyObj used in SWIG to increment ref count on output
      virtual PyObj * getPyObj() const;
      virtual std::string getAsString();
      virtual std::string dump();
      virtual std::string typeName() {return "YACS__ENGINE__OutputPyPort";}
      virtual std::string valToStr();
      virtual void valFromStr(std::string valstr);
    protected:
      PyObject* _data;
    };



  }
}

#endif
