
#include "RuntimeSALOME.hxx"
#include "guiContext.hxx"
#include "Proc.hxx"
#include "commandsProc.hxx"
#include "guiObservers.hxx"

#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;

using namespace YACS;
using namespace YACS::ENGINE;
using namespace YACS::HMI;

GuiContext * GuiContext::_current = 0;

GuiContext::GuiContext()
  : Subject(0)
{
  _builtinCatalog = getSALOMERuntime()->getBuiltinCatalog();
  _sessionCatalog = 0;
  _invoc = new ProcInvoc();
  _proc = 0;
  _subjectProc = 0;
  _numItem = 0;
  _selectedCanvasItem = 0;

  _canvas = new QCanvas(800,2000);
  _widgetStack = 0;
  _mapOfSubjectNode.clear();
  _mapOfSubjectDataPort.clear();
  _mapOfSubjectLink.clear();
}

GuiContext::~GuiContext()
{
  delete _invoc;
  _subjectProc->clean();
  delete _subjectProc;
}

void GuiContext::setProc(YACS::ENGINE::Proc* proc)
{
  if (_subjectProc)
    {
      Subject::erase(_subjectProc);
      _subjectProc = 0;
    }
  _proc = proc;
  _mapOfSubjectNode.clear();
  _mapOfSubjectDataPort.clear();
  _mapOfSubjectLink.clear();
  _mapOfSubjectComponent.clear();
  _subjectProc = new SubjectProc(proc, this);
  _mapOfSubjectNode[static_cast<Node*>(proc)] = _subjectProc;
  update(YACS::HMI::NEWROOT, 0, _subjectProc);
  _subjectProc->loadProc();
}
