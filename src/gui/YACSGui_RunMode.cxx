
#include "YACSGui_RunMode.h"
#include "YACSGui_TreeViewItem.h"
#include "YACSGui_Executor.h"
#include "Executor.hxx"
#include "Proc.hxx"
#include "ComposedNode.hxx"
#include "ElementaryNode.hxx"

#include <qlistview.h>
#include <qcheckbox.h>
#include <qradiobutton.h>
#include <qlabel.h>

#include <cassert>
#include <set>
#include "utilities.h"

using namespace std;
using namespace YACS::ENGINE;

YACSGui_RunMode::YACSGui_RunMode( YACSGui_Executor* guiExec,
                                  QWidget* parent,
                                  const char* name,
                                  bool modal,
                                  WFlags fl)
  : runMode(parent, name, modal, fl)
{
  _guiExec = guiExec;
  assert(_guiExec);
  onNotifyStatus(YACS::NOTYETINITIALIZED);
  resetTreeNode();
  listView_nextSteps->clear();
}

void YACSGui_RunMode::resetTreeNode()
{
  //MESSAGE("YACSGui_RunMode::resetTreeNode");
  _proc = _guiExec->getProc();
  assert(_proc);
  
  listView_breakpoints->clear();
  listView_breakpoints->addColumn( "state", 100);
  listView_breakpoints->setRootIsDecorated( TRUE );

  rb_modeContinue->setChecked(true);

  YACSGui_ComposedNodeViewItem* item =  new YACSGui_ComposedNodeViewItem(listView_breakpoints,
									 "root" );
  _mapListViewItem[_proc->getNumId()] = item;
  addTreeNode(item, (ComposedNode*)(_proc));
  listView_breakpoints->setOpen(item, true);
  //listView_breakpoints->setResizeMode(QListView::LastColumn);
  //resize(sizeHint().width(), height());
}

YACSGui_RunMode::~YACSGui_RunMode()
{
  qWarning("YACSGui_RunMode::~YACSGui_RunMode");
}

void YACSGui_RunMode::addTreeNode(YACSGui_ComposedNodeViewItem *parent,
                                  YACS::ENGINE::ComposedNode* father)
{
  set<Node *> setOfNode= father->edGetDirectDescendants();
  for(set<Node *>::iterator iter=setOfNode.begin();iter!=setOfNode.end();iter++)
    {
      if (ElementaryNode* elemNode = dynamic_cast<ElementaryNode*> (*iter) )
        {
          YACSGui_ElementaryNodeViewItem* item = 
	    new YACSGui_ElementaryNodeViewItem(parent,
					       (*iter)->getQualifiedName(),
					       YACSGui_ElementaryNodeViewItem::CheckBox,
					       elemNode);
          _mapListViewItem[(*iter)->getNumId()] = item;
        }
      else
        {
          YACSGui_ComposedNodeViewItem* item = 
	    new YACSGui_ComposedNodeViewItem(parent,
					     (*iter)->getQualifiedName());
          _mapListViewItem[(*iter)->getNumId()] = item;
          addTreeNode(item, (ComposedNode*)(*iter));
        }
    }
}

void YACSGui_RunMode::onResume()
{
  qWarning("YACSGui_RunMode::onResume");
  printf(">> ::onResume begin -->\n");
  if (_guiExec->checkEndOfDataFlow(false))   // --- finished or not started
    {
      printf(">> before run\n");
      //resetTreeNode();
      _guiExec->runDataflow(true);           // --- remote run only
      printf(">> after run\n");
    }
  else {                                     // --- in progress (suspended or running)
    printf(">> before resume\n");
    _guiExec->resumeDataflow();
    printf(">> after resume\n");
  }
  printf(">> ::onResume end <--\n");
}

void YACSGui_RunMode::onPause()
{
  qWarning("YACSGui_RunMode::onPause");
  if (! _guiExec->checkEndOfDataFlow(false)) // --- in progress (suspended or running) 
    _guiExec->suspendDataflow();
}

void YACSGui_RunMode::onStop()
{
  qWarning("YACSGui_RunMode::onStop");
  _guiExec->killDataflow();
  //close();
}

void YACSGui_RunMode::onModeContinue()
{
  qWarning("YACSGui_RunMode::onModeContinue");
  _guiExec->setContinueMode();
//   QListViewItemIterator it( listView_breakpoints );
//   for ( ; it.current(); ++it )
//     if (YACSGui_ElementaryNodeViewItem* nodeit = dynamic_cast<YACSGui_ElementaryNodeViewItem*> (*it))
//       nodeit->setEnabled(false);
}

void YACSGui_RunMode::onModeStepByStep()
{
  qWarning("YACSGui_RunMode::onModeStepByStep");
  _guiExec->setStepByStepMode();
}

void YACSGui_RunMode::onModeBreakpoints()
{
  qWarning("YACSGui_RunMode::onModeBreakpoints");
  _guiExec->setBreakpointMode();
}

void YACSGui_RunMode::onDismiss()
{
  qWarning("YACSGui_RunMode::onDismiss");
  done(0);
}

void YACSGui_RunMode::onStopOnError()
{
  qWarning("YACSGui_RunMode::onStopOnError");
  _guiExec->setStopOnError(chk_saveState->isChecked());
}

void YACSGui_RunMode::onSaveState()
{
  qWarning("YACSGui_RunMode::onSaveState");
  _saveState = chk_saveState->isChecked();
}

void YACSGui_RunMode::onAllNextToRun()
{
  qWarning("YACSGui_RunMode::onAllNextToRun");
  list<string> nextStepList;
  QListViewItemIterator it(listView_nextSteps);
//   for ( ; it.current(); ++it )
  while(it.current())
    {
      ((QCheckListItem*)it.current())->setOn(true);
      string nodeName = it.current()->text(0);
      MESSAGE(nodeName);
      nextStepList.push_back(nodeName);
      ++it;
    }
  _guiExec->setNextStepList(nextStepList);
}

void YACSGui_RunMode::onRemoveAllNextToRun()
{
  qWarning("YACSGui_RunMode::onRemoveAllNextToRun");
  list<string> nextStepList;
  QListViewItemIterator it(listView_nextSteps);
  while(it.current())
    {
      ((QCheckListItem*)it.current())->setOn(false);
      ++it;
    }
  _guiExec->setNextStepList(nextStepList);
}

void YACSGui_RunMode::onBreakpointClicked(QListViewItem *item)
{
  qWarning("YACSGui_RunMode::onBreakpointClicked");
  YACSGui_ElementaryNodeViewItem* elemNodeItem = dynamic_cast<YACSGui_ElementaryNodeViewItem*>(item);
  if (elemNodeItem)
    {
      MESSAGE("click on node " << elemNodeItem->getNode()->getQualifiedName());
      if (elemNodeItem->isOn())
        {
          if (rb_modeContinue->isChecked())
            {
              onModeBreakpoints();
              rb_modeBreakpoint->setChecked(true);
            }
          _breakpointSet.insert(elemNodeItem->getNode()->getNumId());
        }
      else
        _breakpointSet.erase(elemNodeItem->getNode()->getNumId());
      list<string> breakpointList;
      for (set<int>::iterator pos = _breakpointSet.begin(); pos != _breakpointSet.end(); ++pos)
        {
          string nodeName =_proc->getChildName(((YACSGui_ElementaryNodeViewItem*)(_mapListViewItem[*pos]))->getNode());
          MESSAGE(nodeName);
          breakpointList.push_back(nodeName);
        }
      _guiExec->setBreakpointList(breakpointList);
    }
}

void YACSGui_RunMode::onNextStepClicked(QListViewItem *item)
{
  qWarning("YACSGui_RunMode::onNextStepClicked");
  list<string> nextStepList;
  MESSAGE("click on node " << item->text(0));
  QListViewItemIterator it(listView_nextSteps);
  for ( ; it.current(); ++it )
    if (((QCheckListItem*)it.current())->isOn())
      {
        string nodeName = it.current()->text(0);
        MESSAGE(nodeName);
        nextStepList.push_back(nodeName);
      }
  _guiExec->setNextStepList(nextStepList);
}

void YACSGui_RunMode::onNotifyStatus(int status)
{
  QString theStatus;
  QColor wbg;
  switch (status)
    {
    case YACS::NOTYETINITIALIZED: wbg.setHsv( 45, 50, 255); theStatus = "Not Yet Initialized"; break;
    case YACS::INITIALISED:       wbg.setHsv( 90, 50, 255); theStatus = "Initialized"; break;
    case YACS::RUNNING:           wbg.setHsv(135, 50, 255); theStatus = "Running"; break;
    case YACS::WAITINGTASKS:      wbg.setHsv(180, 50, 255); theStatus = "Waiting Tasks"; break;
    case YACS::PAUSED:            wbg.setHsv(225, 50, 255); theStatus = "Paused"; break;
    case YACS::FINISHED:          wbg.setHsv(270, 50, 255); theStatus = "Finished"; break;
    case YACS::STOPPED:           wbg.setHsv(315, 50, 255); theStatus = "Stopped"; break;
    default:                      wbg.setHsv(360, 50, 255); theStatus = "Status Unknown";

    }
  lab_status->setBackgroundColor(wbg);
  lab_status->setText(theStatus);
}

void YACSGui_RunMode::onNotifyNextSteps(std::list<std::string> nextSteps)
{
  listView_nextSteps->clear();
  _mapNextSteps.clear();
  listView_nextSteps->setRootIsDecorated( TRUE );
  while (! nextSteps.empty())
    {
      QCheckListItem *item = new QCheckListItem(listView_nextSteps,
                                                nextSteps.front().c_str(),
                                                QCheckListItem::CheckBox );
      _mapNextSteps[item] = nextSteps.front();
      item->setOn(true);
      nextSteps.pop_front();
    }
}

void YACSGui_RunMode::onNotifyNodeStatus(int nodeId, int status)
{
  //MESSAGE("YACSGui_RunMode::onNotifyNodeStatus " << nodeId << " " << status );
  QListViewItem* it = _mapListViewItem[nodeId];
  if (!it) return;
  YACSGui_ElementaryNodeViewItem* itn = dynamic_cast<YACSGui_ElementaryNodeViewItem*>(it);
  if (itn)
    {
      itn->setState(status);
      return;
    }
  YACSGui_ComposedNodeViewItem* itc = dynamic_cast<YACSGui_ComposedNodeViewItem*>(it);
  if (itc)
    {
      itc->setState(status);
      return;
    }
  return;
}
