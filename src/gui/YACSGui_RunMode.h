#ifndef _MYRUNMODE_HXX_
#define _MYRUNMODE_HXX_

#include "runmode.h"
#include "ComposedNode.hxx"
#include "Proc.hxx"
#include "define.hxx"

#include <qlistview.h>
#include <list>
#include <string>
#include <set>

class YACSGui_Executor;
class YACS::ENGINE::ElementaryNode;

class ComposedNodeViewItem: public QListViewItem
{
 public:
  ComposedNodeViewItem(QListView *parent, QString label);
  ComposedNodeViewItem(QListViewItem *parent, QString label);
  void setState(int state);
  virtual void paintCell( QPainter *p, const QColorGroup &cg,
                          int column, int width, int alignment );

 protected:
  int _state;
  QColor _cf;
};

class NodeViewItem: public QCheckListItem
{
 public:
  NodeViewItem(QListView *parent,
               const QString &text,
               Type tt = RadioButtonController,
               YACS::ENGINE::ElementaryNode *node = 0);
  NodeViewItem(QListViewItem *parent,
               const QString &text,
               Type tt = RadioButtonController,
               YACS::ENGINE::ElementaryNode *node = 0);
  void setState(int state); 
  virtual void paintCell( QPainter *p, const QColorGroup &cg,
                          int column, int width, int alignment );
  YACS::ENGINE::ElementaryNode* getNode() { return _node;};

 protected:
  int _state;
  QColor _cf;
  YACS::ENGINE::ElementaryNode *_node;
};

class YACSGui_RunMode : public runMode
{
  Q_OBJECT

public:
  YACSGui_RunMode(YACSGui_Executor* guiExec,
                  QWidget* parent = 0,
                  const char* name = 0,
                  bool modal = FALSE,
                  WFlags fl = 0 );
  ~YACSGui_RunMode();

 public slots:
  virtual void onResume();
  virtual void onPause();
  virtual void onStop();
  virtual void onModeContinue();
  virtual void onModeStepByStep();
  virtual void onModeBreakpoints();
  virtual void onDismiss();
  virtual void onStopOnError();
  virtual void onSaveState();
  virtual void onAllNextToRun();
  virtual void onRemoveAllNextToRun();
  virtual void onBreakpointClicked(QListViewItem *item);
  virtual void onNextStepClicked(QListViewItem *item);

  virtual void onNotifyStatus(int status);
  virtual void onNotifyNodeStatus(int nodeId, int status);
  virtual void onNotifyNextSteps(std::list<std::string> nextSteps);

 protected:
  void resetTreeNode();
  void addTreeNode(ComposedNodeViewItem *parent,
                   YACS::ENGINE::ComposedNode* father);

  YACSGui_Executor* _guiExec;
  YACS::ENGINE::Proc* _proc;
  bool _saveState;
  std::map<int, QListViewItem*> _mapListViewItem;
  std::map<QCheckListItem*, std::string> _mapNextSteps;
  std::set<int> _breakpointSet;
};

#endif
