
#include "YACSGui_RunMode.h"
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



ComposedNodeViewItem::ComposedNodeViewItem(QListView *parent, QString label)
  : QListViewItem(parent, label)
{
  _cf = Qt::black;
}

ComposedNodeViewItem::ComposedNodeViewItem(QListViewItem *parent, QString label)
  : QListViewItem(parent, label)
{
  _cf = Qt::black;
}

void ComposedNodeViewItem::paintCell( QPainter *p, const QColorGroup &cg,
                                      int column, int width, int alignment )
{
  //MESSAGE("ComposedNodeViewItem::paintCell " << column);
  QColorGroup _cg( cg );
  QColor c = _cg.text();
  if (column == 1) _cg.setColor( QColorGroup::Text, _cf );
  QListViewItem::paintCell( p, _cg, column, width, alignment );
  if (column == 1) _cg.setColor( QColorGroup::Text, c );
}

void ComposedNodeViewItem::setState(int state)
{
  //MESSAGE("ComposedNodeViewItem::setState");
  _state = state;
  switch (_state)
    {
    case YACS::UNDEFINED:    _cf=Qt::lightGray;       setText(1,"UNDEFINED");    repaint(); break;
    case YACS::INITED:       _cf=Qt::gray;            setText(1,"INITED");       repaint(); break;
    case YACS::TOLOAD:       _cf=Qt::darkYellow;      setText(1,"TOLOAD");       repaint(); break;
    case YACS::LOADED:       _cf=Qt::darkMagenta;     setText(1,"LOADED");       repaint(); break;
    case YACS::TOACTIVATE:   _cf=Qt::darkCyan;        setText(1,"TOACTIVATE");   repaint(); break;
    case YACS::ACTIVATED:    _cf=Qt::darkBlue;        setText(1,"ACTIVATED");    repaint(); break;
    case YACS::DESACTIVATED: _cf=Qt::gray;            setText(1,"DESACTIVATED"); repaint(); break;
    case YACS::DONE:         _cf=Qt::darkGreen;       setText(1,"DONE");         repaint(); break;
    case YACS::SUSPENDED:    _cf=Qt::gray;            setText(1,"SUSPENDED");    repaint(); break;
    case YACS::LOADFAILED:   _cf.setHsv(320,255,255); setText(1,"LOADFAILED");   repaint(); break;
    case YACS::EXECFAILED:   _cf.setHsv( 20,255,255); setText(1,"EXECFAILED");   repaint(); break;
    case YACS::PAUSE:        _cf.setHsv(180,255,255); setText(1,"PAUSE");        repaint(); break;
    case YACS::INTERNALERR:  _cf.setHsv(340,255,255); setText(1,"INTERNALERR");  repaint(); break;
    case YACS::DISABLED:     _cf.setHsv( 40,255,255); setText(1,"DISABLED");     repaint(); break;
    case YACS::FAILED:       _cf.setHsv( 20,255,255); setText(1,"FAILED");       repaint(); break;
    case YACS::ERROR:        _cf.setHsv(  0,255,255); setText(1,"ERROR");        repaint(); break;
    default:                 _cf=Qt::lightGray; repaint();
    }
}


NodeViewItem::NodeViewItem(QListView *parent,
                           const QString &text,
                           Type tt,
                           YACS::ENGINE::ElementaryNode *node)
  : QCheckListItem(parent, text, tt)
{
  _cf = Qt::green;
  _node = node;
}

NodeViewItem::NodeViewItem(QListViewItem *parent,
                           const QString &text,
                           Type tt,
                           YACS::ENGINE::ElementaryNode *node)
  : QCheckListItem(parent, text, tt)
{
  _cf = Qt::green;
  _node = node;
}

void NodeViewItem::paintCell( QPainter *p, const QColorGroup &cg,
                              int column, int width, int alignment )
{
  QColorGroup _cg( cg );
  QColor c = _cg.text();
  if (column == 1) _cg.setColor( QColorGroup::Text, _cf );  
  QCheckListItem::paintCell( p, _cg, column, width, alignment );
  if (column == 1) _cg.setColor( QColorGroup::Text, c );
}

void NodeViewItem::setState(int state)
{
  //MESSAGE("NodeViewItem::setState");
  _state = state;
  switch (_state)
    {
    case YACS::UNDEFINED:    _cf=Qt::lightGray;       setText(1,"UNDEFINED");    repaint(); break;
    case YACS::INITED:       _cf=Qt::gray;            setText(1,"INITED");       repaint(); break;
    case YACS::TOLOAD:       _cf=Qt::darkYellow;      setText(1,"TOLOAD");       repaint(); break;
    case YACS::LOADED:       _cf=Qt::darkMagenta;     setText(1,"LOADED");       repaint(); break;
    case YACS::TOACTIVATE:   _cf=Qt::darkCyan;        setText(1,"TOACTIVATE");   repaint(); break;
    case YACS::ACTIVATED:    _cf=Qt::darkBlue;        setText(1,"ACTIVATED");    repaint(); break;
    case YACS::DESACTIVATED: _cf=Qt::gray;            setText(1,"DESACTIVATED"); repaint(); break;
    case YACS::DONE:         _cf=Qt::darkGreen;       setText(1,"DONE");         repaint(); break;
    case YACS::SUSPENDED:    _cf=Qt::gray;            setText(1,"SUSPENDED");    repaint(); break;
    case YACS::LOADFAILED:   _cf.setHsv(320,255,255); setText(1,"LOADFAILED");   repaint(); break;
    case YACS::EXECFAILED:   _cf.setHsv( 20,255,255); setText(1,"EXECFAILED");   repaint(); break;
    case YACS::PAUSE:        _cf.setHsv(180,255,255); setText(1,"PAUSE");        repaint(); break;
    case YACS::INTERNALERR:  _cf.setHsv(340,255,255); setText(1,"INTERNALERR");  repaint(); break;
    case YACS::DISABLED:     _cf.setHsv( 40,255,255); setText(1,"DISABLED");     repaint(); break;
    case YACS::FAILED:       _cf.setHsv( 20,255,255); setText(1,"FAILED");       repaint(); break;
    case YACS::ERROR:        _cf.setHsv(  0,255,255); setText(1,"ERROR");        repaint(); break;
    default:                 _cf=Qt::lightGray; repaint();
   }
}


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

  ComposedNodeViewItem* item =  new ComposedNodeViewItem(listView_breakpoints,
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

void YACSGui_RunMode::addTreeNode(ComposedNodeViewItem *parent,
                                  YACS::ENGINE::ComposedNode* father)
{
  set<Node *> setOfNode= father->edGetDirectDescendants();
  for(set<Node *>::iterator iter=setOfNode.begin();iter!=setOfNode.end();iter++)
    {
      if (ElementaryNode* elemNode = dynamic_cast<ElementaryNode*> (*iter) )
        {
          NodeViewItem* item =  new NodeViewItem(parent,
                                                 (*iter)->getQualifiedName(),
                                                 NodeViewItem::CheckBox,
                                                 elemNode);
          _mapListViewItem[(*iter)->getNumId()] = item;
        }
      else
        {
          ComposedNodeViewItem* item =  new ComposedNodeViewItem(parent,
                                                                 (*iter)->getQualifiedName());
          _mapListViewItem[(*iter)->getNumId()] = item;
          addTreeNode(item, (ComposedNode*)(*iter));
        }
    }
}

void YACSGui_RunMode::onResume()
{
  qWarning("YACSGui_RunMode::onResume");
  if (_guiExec->checkEndOfDataFlow(false))   // --- finished or not started
    {
      //resetTreeNode();
      _guiExec->runDataflow(true);           // --- remote run only
    }
  else                                       // --- in progress (suspended or running)
    _guiExec->resumeDataflow();
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
//     if (NodeViewItem* nodeit = dynamic_cast<NodeViewItem*> (*it))
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
  NodeViewItem* elemNodeItem = dynamic_cast<NodeViewItem*>(item);
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
          string nodeName =_proc->getChildName(((NodeViewItem*)(_mapListViewItem[*pos]))->getNode());
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
  NodeViewItem* itn = dynamic_cast<NodeViewItem*>(it);
  if (itn)
    {
      itn->setState(status);
      return;
    }
  ComposedNodeViewItem* itc = dynamic_cast<ComposedNodeViewItem*>(it);
  if (itc)
    {
      itc->setState(status);
      return;
    }
  return;
}
