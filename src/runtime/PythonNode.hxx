
#ifndef _PYTHONNODE_HXX_
#define _PYTHONNODE_HXX_

#include "InlineNode.hxx"
#include <Python.h>

namespace YACS
{
  namespace ENGINE
  {
    class PythonNode : public InlineNode 
    {
    protected:
      Node *simpleClone(ComposedNode *father, bool editionOnly) const;
    public:
      PythonNode(const PythonNode& other, ComposedNode *father);
      PythonNode(const std::string& name);
      virtual ~PythonNode();
      virtual void execute();
      virtual void load();
      PythonNode* cloneNode(const std::string& name);
      static const char KIND[];
      static const char IMPL_NAME[];
      virtual std::string typeName() {return "YACS__ENGINE__PythonNode";}
    protected:
      PyObject* _context;
    };

    class PyFuncNode : public InlineFuncNode 
    {
    protected:
      Node *simpleClone(ComposedNode *father, bool editionOnly) const;
    public:
      PyFuncNode(const PyFuncNode& other, ComposedNode *father);
      PyFuncNode(const std::string& name);
      virtual ~PyFuncNode();
      virtual void execute();
      virtual void load();
      PyFuncNode* cloneNode(const std::string& name);
      virtual std::string typeName() {return "YACS__ENGINE__PyFuncNode";}
    protected:
      PyObject* _context;
      PyObject* _pyfunc;
    };
  }
}

#endif
