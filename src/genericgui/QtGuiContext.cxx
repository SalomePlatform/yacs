//  Copyright (C) 2006-2008  CEA/DEN, EDF R&D
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
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

