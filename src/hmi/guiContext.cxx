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

#include "RuntimeSALOME.hxx"
#include "guiContext.hxx"
#include "Proc.hxx"
#include "Container.hxx"
#include "ComponentInstance.hxx"
#include "commandsProc.hxx"
#include "guiObservers.hxx"

//#define _DEVDEBUG_
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
  _procCatalog = 0;
  _currentCatalog = 0;

  _invoc = new ProcInvoc();
  _proc = 0;
  _subjectProc = 0;
  _numItem = 0;
  _isNotSaved = false;
  _isLoading = false;

  _mapOfSubjectNode.clear();
  _mapOfSubjectDataPort.clear();
  _mapOfSubjectLink.clear();
  _mapOfSubjectControlLink.clear();
  _mapOfSubjectComponent.clear();
  _mapOfSubjectContainer.clear();
  _mapOfSubjectDataType.clear();
  _mapOfExecSubjectNode.clear();
  _mapOfLastComponentInstance.clear();
  _lastErrorMessage ="";
  _xmlSchema ="";
  _YACSEngineContainer = pair<string, string>("YACSServer","localhost");
}

GuiContext::~GuiContext()
{
  if (_invoc) delete _invoc;
  _invoc = 0;
  if (_subjectProc)
    {
      _subjectProc->clean();
      delete _subjectProc;
      _subjectProc = 0;
    }
  _current = 0;
}

void GuiContext::setProc(YACS::ENGINE::Proc* proc)
{
  DEBTRACE("GuiContext::setProc ");
  if (_subjectProc)
    {
      Subject::erase(_subjectProc);
      _subjectProc = 0;
    }
  _proc = proc;
  _mapOfSubjectNode.clear();
  _mapOfSubjectDataPort.clear();
  _mapOfSubjectLink.clear();
  _mapOfSubjectControlLink.clear();
  _mapOfSubjectComponent.clear();
  _mapOfSubjectContainer.clear();
  _mapOfSubjectDataType.clear();
  _mapOfLastComponentInstance.clear();
  _lastErrorMessage ="";

  _subjectProc = new SubjectProc(proc, this);
  _mapOfSubjectNode[static_cast<Node*>(proc)] = _subjectProc;
  update(YACS::HMI::NEWROOT, 0, _subjectProc);
  _subjectProc->loadProc();
  update(YACS::HMI::ENDLOAD, 0, _subjectProc);
}

long GuiContext::getNewId(YACS::HMI::TypeOfElem type)
{
  return _numItem++;
}
