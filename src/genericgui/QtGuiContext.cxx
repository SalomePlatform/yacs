
#include "QtGuiContext.hxx"

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;
using namespace YACS::ENGINE;
using namespace YACS::HMI;

QtGuiContext * QtGuiContext::_QtCurrent = 0;


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
  _fileName = QString();
  _mapOfSchemaItem.clear();
  _mapOfSceneItem.clear();
  _setOfModifiedSubjects.clear();
}

QtGuiContext::~QtGuiContext()
{
}

void QtGuiContext::setProc(YACS::ENGINE::Proc* proc)
{
  _mapOfSchemaItem.clear();
  _mapOfSceneItem.clear();
  GuiContext::setProc(proc);
}

