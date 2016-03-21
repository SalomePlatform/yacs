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

#include "EditionProc.hxx"

#include "QtGuiContext.hxx"
#include "LinkInfo.hxx"
#include "Logger.hxx"
#include <QComboBox>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

#include <cassert>

using namespace std;

using namespace YACS;
using namespace YACS::HMI;
using namespace YACS::ENGINE;

EditionProc::EditionProc(Subject* subject,
                         QWidget* parent,
                         const char* name)
  : EditionBloc(subject, parent, name)
{
  DEBTRACE("EditionProc::EditionProc");
  _statusLog = new QTextEdit(this);
  _wid->gridLayout1->addWidget(_statusLog);
  _errorLog = "";
  _modifLog = "";

  if (!QtGuiContext::getQtCurrent()->isEdition())
    {
      QHBoxLayout* hbox = new QHBoxLayout();
      QLabel* la = new QLabel("Shutdown level:",this);
      QComboBox* cb = new QComboBox(this);
      cb->addItem("0",0 );
      cb->addItem("1",1 );
      cb->addItem("2",2 );
      cb->addItem("3",3 );
      int level=1;
      if (QtGuiContext::getQtCurrent()->getGuiExecutor())
        level=QtGuiContext::getQtCurrent()->getGuiExecutor()->getShutdownLevel();
      DEBTRACE(level);
      cb->setCurrentIndex(level);
      connect(cb, SIGNAL(currentIndexChanged(int)), this, SLOT(onLevelChange(int)));
      hbox->addWidget(la);
      hbox->addWidget(cb);
      _wid->gridLayout1->addLayout(hbox,4,0);
    }
}

EditionProc::~EditionProc()
{
  DEBTRACE("EditionProc::~EditionProc");
}

void EditionProc::update(GuiEvent event, int type, Subject* son)
{
  DEBTRACE("EditionProc::update " << GuiObserver::eventName(event));
  EditionBloc::update(event, type, son);
  string statusLog = "";
  switch (event)
    {
    case EDIT:
      if (QtGuiContext::getQtCurrent()->_setOfModifiedSubjects.empty())
        _modifLog = "";
      else
        _modifLog = "--- some elements are modified and not taken into account. Confirmation or annulation required ---\n";
    case UPDATE:
      synchronize();
      break;
    case UPDATEPROGRESS:
        {
          if(type != YACS::FINISHED)break;
          if (!QtGuiContext::getQtCurrent()) break;
          if (!QtGuiContext::getQtCurrent()->getGuiExecutor()) break;
          YACS::ENGINE::Proc* proc = QtGuiContext::getQtCurrent()->getProc();
          _errorLog = QtGuiContext::getQtCurrent()->getGuiExecutor()->getErrorReport(proc);
          _statusLog->setText(QString::fromStdString(_errorLog));
          break;
        }
    default:
      ;
    }

}

void EditionProc::synchronize()
{
  DEBTRACE("EditionProc::synchronize");
  YACS::ENGINE::Proc* proc = QtGuiContext::getQtCurrent()->getProc();
  Logger* logger = 0;
  string statusLog = "";
  
  if (!QtGuiContext::getQtCurrent()->isEdition())
    return;
  if (!proc->isValid())
    {
      _errorLog = "--- YACS schema is not valid ---\n\n";
      _errorLog += proc->getErrorReport();
      DEBTRACE(_errorLog);
    }
  else
    {
      // --- Check consistency
      LinkInfo info(LinkInfo::ALL_DONT_STOP);
      _errorLog="";
      try
        {
          proc->checkConsistency(info);
          if (info.areWarningsOrErrors())
            _errorLog += info.getGlobalRepr();
          else
            {
              _errorLog += "--- No Validity Errors ---\n";
              _errorLog += "--- No Consistency Errors ---\n";
            }
        }
      catch (Exception &ex)
        {
          _errorLog = "--- YACS schema has consistency errors ---\n\n";
          _errorLog += ex.what();
        }
      DEBTRACE(_errorLog);
    }
  // --- Add initial logger info
  logger = proc->getLogger("parser");
  if (!logger->isEmpty())
    {
      _errorLog += "--- Original file import log ---\n";
      _errorLog += logger->getStr();  
    }
  statusLog = _modifLog + _errorLog;
  _statusLog->setText(statusLog.c_str());
}

void EditionProc::onLevelChange(int index)
{
  DEBTRACE("EditionProc::onLevelChange " << index);
  if (QtGuiContext::getQtCurrent()->getGuiExecutor())
    QtGuiContext::getQtCurrent()->getGuiExecutor()->setShutdownLevel(index);
}
