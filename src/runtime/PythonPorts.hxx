// Copyright (C) 2006-2016  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#ifndef _PYTHONPORTS_HXX_
#define _PYTHONPORTS_HXX_

#include <Python.h>

#include "YACSRuntimeSALOMEExport.hxx"
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

    class InterpreterSaveThread {
    public:
      inline InterpreterSaveThread() {
        tstate_ = PyEval_SaveThread();
      }
      inline ~InterpreterSaveThread() {
        PyEval_RestoreThread(tstate_);
      }
      inline void lock() {
        PyEval_RestoreThread(tstate_);
      }
      inline void unlock() {
        tstate_ = PyEval_SaveThread();
      }
    private:
      PyThreadState* tstate_;
    };


    typedef PyObject PyObj;

/*! \brief Class for Python Ports
 *
 * \ingroup Ports
 *
 * \see PythonNode
 */
    class YACSRUNTIMESALOME_EXPORT InputPyPort : public InputPort
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
      virtual std::string getHumanRepr();
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

    class YACSRUNTIMESALOME_EXPORT OutputPyPort : public OutputPort
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
