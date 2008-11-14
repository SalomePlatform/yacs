
#include "EditionProc.hxx"

#include "QtGuiContext.hxx"
#include "LinkInfo.hxx"
#include "Logger.hxx"

#define _DEVDEBUG_
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
}

EditionProc::~EditionProc()
{
  DEBTRACE("EditionProc::~EditionProc");
}

void EditionProc::update(GuiEvent event, int type, Subject* son)
{
  DEBTRACE("EditionProc::update");
  EditionBloc::update(event, type, son);
  YACS::ENGINE::Proc* proc = QtGuiContext::getQtCurrent()->getProc();
  Logger* logger = 0;
  string statusLog = "";
  switch (event)
    {
    case EDIT:
      if (QtGuiContext::getQtCurrent()->_setOfModifiedSubjects.empty())
        _modifLog = "";
      else
        _modifLog = "--- some elements are modified and not taken into account. Confirmation or annulation required ---\n";
      statusLog = _modifLog + _errorLog;
      _statusLog->setText(statusLog.c_str());
      break;
    case UPDATE:
      if (!proc->isValid())
        {
          _errorLog = "--- YACS schema is not valid ---\n\n";
          _errorLog += proc->getErrorReport();
        }
      else
        {
          // --- Check consistency
          LinkInfo info(LinkInfo::ALL_DONT_STOP);
          proc->checkConsistency(info);
          if (info.areWarningsOrErrors())
            _errorLog = info.getGlobalRepr();
          else
            {
              _errorLog = "--- No Validity Errors ---\n";
              _errorLog += "--- No Consistency Errors ---\n";
            }
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
      break;
    default:
      ;
    }

}
