
#ifndef _VISITORSAVEGUISCHEMA_HXX_
#define _VISITORSAVEGUISCHEMA_HXX_

#include "VisitorSaveSalomeSchema.hxx"

namespace YACS
{
  namespace ENGINE
  {
     class Proc;
  };

  namespace HMI
  {
    class VisitorSaveGuiSchema : public YACS::ENGINE::VisitorSaveSalomeSchema
    {
    public:
      VisitorSaveGuiSchema(YACS::ENGINE::Proc* proc);
      virtual ~VisitorSaveGuiSchema();
      
      virtual void visitProc(YACS::ENGINE::Proc *node);
      
    protected:
      virtual void writePresentation(YACS::ENGINE::Proc *proc);
      virtual void writeLinks(YACS::ENGINE::Proc *proc);
    };

  };
};

#endif
