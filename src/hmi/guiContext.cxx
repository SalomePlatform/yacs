
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

GuiContext::GuiContext(YACS::ENGINE::Proc* proc)
  : Subject(0), _proc(proc)
{
  _builtinCatalog = getSALOMERuntime()->getBuiltinCatalog();
  _subjectProc = new SubjectProc(_proc, this);
  _invoc = new ProcInvoc(_proc);
  _numItem = 0;
  _selectedCanvasItem = 0;

  _canvas = new QCanvas(800,600);
  _widgetStack = 0;
}

GuiContext::~GuiContext()
{
  delete _invoc;
  delete _subjectProc;
}
