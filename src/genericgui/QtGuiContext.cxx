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

#include "QtGuiContext.hxx"
#include <QClipboard>
#include "ItemMimeData.hxx"

//#define _DEVDEBUG_
#include "YacsTrace.hxx"
#include "chrono.hxx"

using namespace std;
using namespace YACS::ENGINE;
using namespace YACS::HMI;

QtGuiContext * QtGuiContext::_QtCurrent = 0;
counters *QtGuiContext::_counters = 0;
bool QtGuiContext::_delayCalc = false;
std::set<QtGuiContext*> QtGuiContext::_setOfContext;

QtGuiContext::QtGuiContext(GenericGui *gmain) : GuiContext()
{
  _gmain = gmain;
  _schemaModel = 0;
  _editTree = 0;
  _selectionModel = 0;
  _scene = 0;
  _view = 0;
  _rootEdit = 0;
  _window = 0;
  _stackedWidget = 0;
  _guiExecutor = 0;
  _selectedSubject = 0;
  _isEdition = true;
  _isLoadingPresentation = false;
  _studyId = 0;
  _fileName = QString();
  _mapOfSchemaItem.clear();
  _mapOfSceneItem.clear();
  _mapOfEditionItem.clear();
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
      _proc->setEdition(false);
      _subjectProc->clean();
      delete _subjectProc;
      _subjectProc = 0;
    }
  _current = 0;
  _QtCurrent = 0;
  _setOfContext.erase(this);
  // Avoid any possible copy from the destroyed schema.
  QClipboard *clipboard = QApplication::clipboard();
  clipboard->setMimeData(NULL);
}

void QtGuiContext::setProc(YACS::ENGINE::Proc* proc)
{
  _mapOfSchemaItem.clear();
  _mapOfSceneItem.clear();
  GuiContext::setProc(proc);
}

YACS::HMI::Subject* QtGuiContext::getSubjectToPaste(bool &isCut)
{
  QClipboard *clipboard = QApplication::clipboard();
  const QMimeData * data=clipboard->mimeData();
  const ItemMimeData* myData = dynamic_cast<const ItemMimeData*>(data);
  Subject * sub=0;

  if(myData)
    {
      sub=myData->getSubject();
      isCut=myData->getControl();
    }

  return sub;
}

void QtGuiContext::setSubjectToCut(YACS::HMI::Subject* sub)
{
  ItemMimeData *mime = new ItemMimeData;
  mime->setSubject(sub);
  mime->setControl(true);
  QClipboard *clipboard = QApplication::clipboard();
  clipboard->setMimeData(mime);
}

void QtGuiContext::setSubjectToCopy(YACS::HMI::Subject* sub)
{
  ItemMimeData *mime = new ItemMimeData;
  mime->setSubject(sub);
  mime->setControl(false);
  QClipboard *clipboard = QApplication::clipboard();
  clipboard->setMimeData(mime);
}
