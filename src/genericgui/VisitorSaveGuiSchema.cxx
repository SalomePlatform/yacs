
#include "VisitorSaveGuiSchema.hxx"
#include "Proc.hxx"

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;
using namespace YACS::HMI;
using namespace YACS::ENGINE;

VisitorSaveGuiSchema::VisitorSaveGuiSchema(YACS::ENGINE::Proc* proc)
  : VisitorSaveSalomeSchema(proc)
{
  DEBTRACE("VisitorSaveGuiSchema::VisitorSaveGuiSchema");
}

VisitorSaveGuiSchema::~VisitorSaveGuiSchema()
{
  DEBTRACE("VisitorSaveGuiSchema::~VisitorSaveGuiSchema");
}

void VisitorSaveGuiSchema::visitProc(YACS::ENGINE::Proc *node)
{
  DEBTRACE("VisitorSaveGuiSchema::visitProc");
  VisitorSaveSalomeSchema::visitProc(node);
}

void VisitorSaveGuiSchema::writePresentation(YACS::ENGINE::Proc *proc)
{
  DEBTRACE("VisitorSaveGuiSchema::writePresentation");
}

void VisitorSaveGuiSchema::writeLinks(YACS::ENGINE::Proc *proc)
{
  DEBTRACE("VisitorSaveGuiSchema::writeLinks");
}
