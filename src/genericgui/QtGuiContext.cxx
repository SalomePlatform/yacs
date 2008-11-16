
#include "QtGuiContext.hxx"

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;
using namespace YACS::ENGINE;
using namespace YACS::HMI;

QtGuiContext * QtGuiContext::_QtCurrent = 0;
std::set<QtGuiContext*> QtGuiContext::_setOfContext;

QtGuiContext::QtGuiContext(GenericGui *gmain) : GuiContext()
{
  _gmain = gmain;
  _schemaModel = 0;
  _editTree = 0;
  _selectionModel = 0;
  _scene = 0;
  _view = 0;
  _window = 0;
  _stackedWidget = 0;
  _guiExecutor = 0;
  _selectedSubject = 0;
  _isEdition = true;
  _studyId = 0;
  _fileName = QString();
  _mapOfSchemaItem.clear();
  _mapOfSceneItem.clear();
  _setOfModifiedSubjects.clear();
  _setOfContext.insert(this);
}

QtGuiContext::~QtGuiContext()
{
  DEBTRACE("QtGuiContext::~QtGuiContext");
  if (_invoc) delete _invoc;
  _invoc = 0;
  if (_subjectProc)
    {
      _subjectProc->clean();
      delete _subjectProc;
      _subjectProc = 0;
    }
  _current = 0;
  _QtCurrent = 0;
  _setOfContext.erase(this);
}

void QtGuiContext::setProc(YACS::ENGINE::Proc* proc)
{
  _mapOfSchemaItem.clear();
  _mapOfSceneItem.clear();
  GuiContext::setProc(proc);
}

