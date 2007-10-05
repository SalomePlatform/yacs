#ifndef _SALOMEPYTHONNODE_HXX_
#define _SALOMEPYTHONNODE_HXX_

#include "ServiceInlineNode.hxx"
#include <Python.h>

namespace YACS
{
  namespace ENGINE
  {
    /*!
     * This class is in charge to deal with python willing to interact with YACS placed container.
     * Typical use is for nodes building GEOM and MESH. The great number of calls needed by this type of application,
     * implies a human impossibility to make the corresponding graph.
    */
    class SalomePythonNode : public ServiceInlineNode 
    {
    protected:
      Node *simpleClone(ComposedNode *father, bool editionOnly) const;
    public:
      SalomePythonNode(const SalomePythonNode& other, ComposedNode *father);
      SalomePythonNode(const std::string& name);
      virtual void execute();
      virtual void load();
      std::string getKind() const;
      ServiceNode *createNode(const std::string &name);
      SalomePythonNode* cloneNode(const std::string& name);
    protected:
      PyObject* _context;
      PyObject* _pyfunc;
    public:
      static const char PLACEMENT_VAR_NAME_IN_INTERP[];
    };
  }
}

#endif
