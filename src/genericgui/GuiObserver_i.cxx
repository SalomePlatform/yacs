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

#include "GuiObserver_i.hxx"
#include "GuiExecutor.hxx"
#include "Proc.hxx"

#include <QApplication>
#include <cassert>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;
using namespace YACS::HMI;
using namespace YACS::ENGINE;

YACSEvent::YACSEvent(std::pair<int,std::string> aYACSEvent)
  : QEvent(QEvent::User)
{
  _event = aYACSEvent;
}


GuiObserver_i::GuiObserver_i(YACS::ENGINE::Proc* guiProc)
{
  DEBTRACE("GuiObserver_i::GuiObserver_i");
  _guiProc = guiProc;
  _engineProc = YACS_ORB::ProcExec::_nil();
  _impl = 0;
  _guiToEngineMap.clear();
  _engineToGuiMap.clear();
}

GuiObserver_i::~GuiObserver_i()
{
  DEBTRACE("GuiObserver_i::~GuiObserver_i");
}

void GuiObserver_i::notifyObserver(CORBA::Long numid, const char* event)
{
  DEBTRACE("GuiObserver_i::notifyObserver " << numid << " " << event);
  pair<int,string> myEvent(numid, event);
  YACSEvent* evt = new YACSEvent(myEvent);
  QApplication::postEvent(_impl, evt);  // Qt will delete it when done
}

void GuiObserver_i::setConversion()
{
  DEBTRACE("GuiObserver_i::setConversion");
  YASSERT(!CORBA::is_nil(_engineProc));
  YACS_ORB::stringArray_var engineNames;
  YACS_ORB::longArray_var engineIds;
  //DEBTRACE("---");
  _engineProc->getIds(engineIds.out(), engineNames.out());
  int iLength = engineIds->length();
  int nLength = engineNames->length();
  if (nLength < iLength) iLength = nLength;
  for(int i=0; i<iLength; i++)
    {
      string aName = "";
      aName = engineNames[i];
      int iEng = engineIds[i];
      DEBTRACE("--- " << aName << " " << iEng);
      if (aName != "_root_")
        {
          int iGui = _guiProc->getChildByName(aName)->getNumId();
          //DEBTRACE("---");
          _guiToEngineMap[iGui] = iEng;
          _engineToGuiMap[iEng] = iGui;
        }
      else
        {
          int iGui = _guiProc->getNumId();
          _guiToEngineMap[iGui] = iEng;
          _engineToGuiMap[iEng] = iGui;
        }
    }
}

void GuiObserver_i::SetImpl(GuiExecutor* impl)
{
  DEBTRACE("GuiObserver_i::SetImpl");
  _impl = impl;
}

void GuiObserver_i::SetRemoteProc(YACS_ORB::ProcExec_ptr engineProc)
{
  DEBTRACE("GuiObserver_i::SetRemoteProc");
  _engineProc = YACS_ORB::ProcExec::_duplicate(engineProc);
}

