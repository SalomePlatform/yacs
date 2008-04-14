#ifndef __VISITORSAVESALOMESCHEMA_HXX__
#define __VISITORSAVESALOMESCHEMA_HXX__

#include "VisitorSaveSchema.hxx"

namespace YACS
{
  namespace ENGINE
  {
    class VisitorSaveSalomeSchema: public VisitorSaveSchema
    {
    public:
      VisitorSaveSalomeSchema(ComposedNode *root);
      virtual ~VisitorSaveSalomeSchema();
      virtual void visitPresetNode(DataNode *node);
      virtual void visitOutNode(DataNode *node);
      virtual void visitStudyInNode(DataNode *node);
      virtual void visitStudyOutNode(DataNode *node);
    protected:
      virtual void writeDataNodeParameters(DataNode *node);
      virtual void writeStudyInNodeParameters(DataNode *node);
      virtual void writeOutNodeParameters(DataNode *node);
      virtual void writeStudyOutNodeParameters(DataNode *node);
    };
  }
}

#endif
