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

#include "Menus.hxx"
#include "QtGuiContext.hxx"
#include "guiObservers.hxx"
#include "DataPort.hxx"
#include "DataNode.hxx"
#include "ServiceNode.hxx"
#include "InlineNode.hxx"

#include <QMenu>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"
#include "chrono.hxx"

using namespace std;
using namespace YACS::HMI;
using namespace YACS::ENGINE;

//=======================================================================================

MenusBase::MenusBase()
{
  _dummyAct = new QAction(tr("Elapse Time Statistics"), this);
//   _dummyAct->setShortcut(tr("Ctrl+y"));
  _dummyAct->setStatusTip(tr("Elapse Time Statistics"));
  _dummyAct->setToolTip(tr("Elapse Time Statistics"));
  connect(_dummyAct, SIGNAL(triggered()), this, SLOT(dummyAction()));
}

MenusBase::~MenusBase()
{
}

void MenusBase::popupMenu(QWidget *caller, const QPoint &globalPos, const QString& m)
{
  QMenu menu(m, caller);
  menu.addAction(_dummyAct);
  menu.exec(globalPos);
}

void MenusBase::dummyAction()
{
  DEBTRACE("MenusBase::dummyAction");
  QtGuiContext::_counters->stats();
}

void MenusBase::foreachAction(QAction* act)
{
  DEBTRACE(act->text().toStdString());
  GenericGui *gmain = QtGuiContext::getQtCurrent()->getGMain();
  gmain->createForEachLoop(act->text().toStdString());
}

void MenusBase::putGraphInForeachAction(QAction* act)
{
  DEBTRACE(act->text().toStdString());
  GenericGui *gmain = QtGuiContext::getQtCurrent()->getGMain();
  gmain->putGraphInForeachLoop(act->text().toStdString());
}

void MenusBase::addHeader(QMenu &m, const QString &h)
{
  m.addAction(_dummyAct);
  _dummyAct->setText(h);
  QFont f = _dummyAct->font();
  f.setBold(true);
  _dummyAct->setFont(f);
  m.addSeparator();
}

void MenusBase::buildForEachMenu(QMenu *m, QActionGroup* actgroup)
{
  QPixmap pixmap;
  pixmap.load("icons:new_foreach_loop_node.png");

  QMenu *ForEachMenu=m->addMenu(QIcon(pixmap),"ForEach Loop");

  Proc* proc = GuiContext::getCurrent()->getProc();
  std::map<std::string, TypeCode*>::const_iterator it = proc->typeMap.begin();
  QAction* act;
  for (; it != proc->typeMap.end(); ++it)
    {
      act=actgroup->addAction((*it).first.c_str());
      ForEachMenu->addAction(act);
    }
}

void MenusBase::addForEachMenu(QMenu *m, QActionGroup* actgroup)
{
  buildForEachMenu(m, actgroup);
  connect(actgroup, SIGNAL(triggered(QAction*)), this, SLOT(foreachAction(QAction*)));
}

void MenusBase::addForEachMenuToPutGraph(QMenu *m, QActionGroup* actgroup)
{
  buildForEachMenu(m, actgroup);
  connect(actgroup, SIGNAL(triggered(QAction*)), this, SLOT(putGraphInForeachAction(QAction*)));
}

//=======================================================================================

ComposedNodeMenu::ComposedNodeMenu() : MenusBase()
{
}

ComposedNodeMenu::~ComposedNodeMenu()
{
}

void ComposedNodeMenu::popupMenu(QWidget *caller, const QPoint &globalPos, const QString& m)
{
  GenericGui *gmain = QtGuiContext::getQtCurrent()->getGMain();
  bool isEdition = QtGuiContext::getQtCurrent()->isEdition();
  QMenu menu(m, caller);
  QActionGroup actgroup(this);
  addHeader(menu, m);
  if (isEdition)
    {
      QMenu *CNmenu = menu.addMenu(tr("Create Node"));
      CNmenu->addAction(gmain->_nodeFromCatalogAct);
      //   CNmenu->addSeparator();
      //   CNmenu->addAction(gmain->_cppNodeAct);
      //   CNmenu->addAction(gmain->_XMLNodeAct);
      CNmenu->addSeparator();
      CNmenu->addAction(gmain->_inlineScriptNodeAct);
      CNmenu->addAction(gmain->_inlineFunctionNodeAct);
      CNmenu->addSeparator();
      CNmenu->addAction(gmain->_inDataNodeAct);
      CNmenu->addAction(gmain->_outDataNodeAct);
      CNmenu->addAction(gmain->_inStudyNodeAct);
      CNmenu->addAction(gmain->_outStudyNodeAct);
      CNmenu->addSeparator();
      CNmenu->addAction(gmain->_blockNodeAct);
      CNmenu->addSeparator();
      CNmenu->addAction(gmain->_FORNodeAct);
      addForEachMenu(CNmenu,&actgroup);
      CNmenu->addAction(gmain->_WHILENodeAct);
      CNmenu->addAction(gmain->_SWITCHNodeAct);
      CNmenu->addAction(gmain->_OptimizerLoopAct);
      menu.addSeparator();
    }
//   menu.addAction(gmain->_toggleSceneItemVisibleAct);
  menu.addAction(gmain->_arrangeLocalNodesAct);
  menu.addAction(gmain->_arrangeRecurseNodesAct);
  menu.addSeparator();
  menu.addAction(gmain->_showOnlyCtrlLinksAct);
  menu.addAction(gmain->_showCtrlLinksAct);
  menu.addAction(gmain->_hideCtrlLinksAct);
  menu.addAction(gmain->_emphasisPortLinksAct);
  menu.addAction(gmain->_deEmphasizeAllAct);
  menu.addSeparator();
  menu.addAction(gmain->_zoomToBlocAct);
  menu.addAction(gmain->_centerOnNodeAct);
  menu.addAction(gmain->_shrinkExpand);
  menu.addAction(gmain->_shrinkExpandChildren);
  menu.addAction(gmain->_shrinkExpandElementaryRecursively);
  menu.addAction(gmain->_computeLinkAct);
//   menu.addAction(gmain->_toggleAutomaticComputeLinkAct);
//   menu.addAction(gmain->_toggleSimplifyLinkAct);
//   menu.addAction(gmain->_toggleForce2NodesLinkAct);
//   menu.addAction(gmain->_toggleAddRowColsAct);
  if (isEdition)
    {
      menu.addSeparator();
      menu.addAction(gmain->_deleteItemAct);
      menu.addAction(gmain->_cutItemAct);
      menu.addAction(gmain->_copyItemAct);
      menu.addAction(gmain->_pasteItemAct);
      menu.addAction(gmain->_putInBlocAct);

      Subject* sub = QtGuiContext::getQtCurrent()->getSelectedSubject();
      SubjectNode *sin = dynamic_cast<SubjectNode*>(sub);
      if(sin && !sin->isValid())
        {
          menu.addSeparator();
          menu.addAction(gmain->_getErrorReportAct);
          menu.addAction(gmain->_getErrorDetailsAct);
        }
    }
  else
    {
      menu.addAction(gmain->_getErrorReportAct);
      menu.addAction(gmain->_getErrorDetailsAct);
    }
  menu.exec(globalPos);
}

//=======================================================================================

ProcMenu::ProcMenu() : MenusBase()
{
}

ProcMenu::~ProcMenu()
{
}

void ProcMenu::popupMenu(QWidget *caller, const QPoint &globalPos, const QString& m)
{
  GenericGui *gmain = QtGuiContext::getQtCurrent()->getGMain();
  bool isEdition = QtGuiContext::getQtCurrent()->isEdition();
  QMenu menu(m, caller);
  addHeader(menu, m);
  QActionGroup actgroup(this);
  QActionGroup actgroup2(this);
  if (isEdition)
    {
      menu.addAction(gmain->_runLoadedSchemaAct);
      menu.addSeparator();
      menu.addAction(gmain->_importDataTypeAct);
      menu.addAction(gmain->_newContainerAct);
      menu.addAction(gmain->_newHPContainerAct);
      QMenu *CNmenu = menu.addMenu(tr("Create Node"));
      CNmenu->addAction(gmain->_nodeFromCatalogAct);
      //   CNmenu->addSeparator();
      //   CNmenu->addAction(gmain->_cppNodeAct);
      //   CNmenu->addAction(gmain->_XMLNodeAct);
      CNmenu->addSeparator();
      CNmenu->addAction(gmain->_inlineScriptNodeAct);
      CNmenu->addAction(gmain->_inlineFunctionNodeAct);
      CNmenu->addSeparator();
      CNmenu->addAction(gmain->_inDataNodeAct);
      CNmenu->addAction(gmain->_outDataNodeAct);
      CNmenu->addAction(gmain->_inStudyNodeAct);
      CNmenu->addAction(gmain->_outStudyNodeAct);
      CNmenu->addSeparator();
      CNmenu->addAction(gmain->_blockNodeAct);
      CNmenu->addSeparator();
      CNmenu->addAction(gmain->_FORNodeAct);
      addForEachMenu(CNmenu,&actgroup);
      CNmenu->addAction(gmain->_WHILENodeAct);
      CNmenu->addAction(gmain->_SWITCHNodeAct);
      CNmenu->addAction(gmain->_OptimizerLoopAct);
      menu.addSeparator();
      menu.addAction(gmain->_pasteItemAct);

      QMenu *PINmenu = menu.addMenu(tr("Put Graph Content in Node"));
      PINmenu->addAction(gmain->_putGraphInBlocAct);
      PINmenu->addSeparator();
      PINmenu->addAction(gmain->_putGraphInForLoopAct);
      addForEachMenuToPutGraph(PINmenu,&actgroup2);
      PINmenu->addAction(gmain->_putGraphInWhileLoopAct);
      PINmenu->addAction(gmain->_putGraphInOptimizerLoopAct);

      menu.addSeparator();
    }
  menu.addAction(gmain->_getYacsContainerLogAct);
  menu.addSeparator();
  menu.addAction(gmain->_showAllLinksAct);
  menu.addAction(gmain->_hideAllLinksAct);
  menu.addAction(gmain->_deEmphasizeAllAct);
  menu.addSeparator();
//   menu.addAction(gmain->_toggleSceneItemVisibleAct);
  menu.addAction(gmain->_arrangeLocalNodesAct);
  menu.addAction(gmain->_arrangeRecurseNodesAct);
  menu.addSeparator();
  menu.addAction(gmain->_zoomToBlocAct);
  menu.addAction(gmain->_centerOnNodeAct);
  menu.addAction(gmain->_shrinkExpand);
  menu.addAction(gmain->_shrinkExpandChildren);
  menu.addAction(gmain->_shrinkExpandElementaryRecursively);
  menu.addAction(gmain->_computeLinkAct);
//   menu.addAction(gmain->_toggleAutomaticComputeLinkAct);
//   menu.addAction(gmain->_toggleSimplifyLinkAct);
//   menu.addAction(gmain->_toggleForce2NodesLinkAct);
//   menu.addAction(gmain->_toggleAddRowColsAct);
  if (!isEdition)
    {
      menu.addAction(gmain->_getErrorReportAct);
      menu.addAction(gmain->_getErrorDetailsAct);
      menu.addAction(gmain->_shutdownProcAct);
    }
  menu.exec(globalPos);
}

//=======================================================================================

NodeMenu::NodeMenu() : MenusBase()
{
}

NodeMenu::~NodeMenu()
{
}

void NodeMenu::popupMenu(QWidget *caller, const QPoint &globalPos, const QString& m)
{
  GenericGui *gmain = QtGuiContext::getQtCurrent()->getGMain();
  bool isEdition = QtGuiContext::getQtCurrent()->isEdition();
  QMenu menu(m, caller);
  addHeader(menu, m);
  if (isEdition)
    {
      menu.addAction(gmain->_deleteItemAct);
      menu.addAction(gmain->_cutItemAct);
      menu.addAction(gmain->_copyItemAct);
      menu.addAction(gmain->_pasteItemAct);
    }
  menu.addSeparator();
  menu.addAction(gmain->_showOnlyCtrlLinksAct);
  menu.addAction(gmain->_showCtrlLinksAct);
  menu.addAction(gmain->_hideCtrlLinksAct);
  menu.addAction(gmain->_emphasisPortLinksAct);
  menu.addAction(gmain->_deEmphasizeAllAct);
  menu.addSeparator();
  menu.addAction(gmain->_zoomToBlocAct);
  menu.addAction(gmain->_centerOnNodeAct);
  menu.exec(globalPos);
}

//=======================================================================================

HeaderNodeMenu::HeaderNodeMenu() : MenusBase()
{
}

HeaderNodeMenu::~HeaderNodeMenu()
{
}

void HeaderNodeMenu::popupMenu(QWidget *caller, const QPoint &globalPos, const QString& m)
{
  GenericGui *gmain = QtGuiContext::getQtCurrent()->getGMain();
  bool isEdition = QtGuiContext::getQtCurrent()->isEdition();
  QMenu menu(m, caller);
  addHeader(menu, m);
  if (isEdition)
    {
      menu.addAction(gmain->_deleteItemAct);
    }
  menu.exec(globalPos);
}

//=======================================================================================

LinkMenu::LinkMenu() : MenusBase()
{
}

LinkMenu::~LinkMenu()
{
}

void LinkMenu::popupMenu(QWidget *caller, const QPoint &globalPos, const QString& m)
{
  GenericGui *gmain = QtGuiContext::getQtCurrent()->getGMain();
  bool isEdition = QtGuiContext::getQtCurrent()->isEdition();
  QMenu menu(m, caller);
  addHeader(menu, m);
  menu.addAction(gmain->_showOnlyLinkAct);
  menu.addAction(gmain->_showLinkAct);
  menu.addAction(gmain->_hideLinkAct);
  menu.addAction(gmain->_emphasisLinkAct);
  menu.addAction(gmain->_deEmphasizeAllAct);
  if (isEdition)
    {
      menu.addSeparator();
      menu.addAction(gmain->_deleteItemAct);
    }
  menu.exec(globalPos);
}

//=======================================================================================

ElementaryNodeMenu::ElementaryNodeMenu() : MenusBase()
{
}

ElementaryNodeMenu::~ElementaryNodeMenu()
{
}

void ElementaryNodeMenu::popupMenu(QWidget *caller, const QPoint &globalPos, const QString& m)
{
  GenericGui *gmain = QtGuiContext::getQtCurrent()->getGMain();
  bool isEdition = QtGuiContext::getQtCurrent()->isEdition();
  QMenu menu(m, caller);
  addHeader(menu, m);
  if (isEdition)
    {
//       Subject* sub = QtGuiContext::getQtCurrent()->getSelectedSubject();
//       SubjectServiceNode *ssn = dynamic_cast<SubjectServiceNode*>(sub);
//       if (ssn)
//         {
//           menu.addAction(gmain->_newContainerAct);
//           menu.addSeparator();
//         }
      Subject* sub = QtGuiContext::getQtCurrent()->getSelectedSubject();
      SubjectNode *sin = dynamic_cast<SubjectNode*>(sub);
      if(sin && !sin->isValid())
        {
          menu.addAction(gmain->_getErrorReportAct);
          menu.addSeparator();
        }
      menu.addAction(gmain->_deleteItemAct);
      menu.addAction(gmain->_cutItemAct);
      menu.addAction(gmain->_copyItemAct);
      menu.addAction(gmain->_pasteItemAct);
      menu.addAction(gmain->_putInBlocAct);
    }
  else
    {
      menu.addAction(gmain->_getErrorReportAct);
      menu.addAction(gmain->_getErrorDetailsAct);
      menu.addAction(gmain->_getContainerLogAct);
    }
  menu.addSeparator();
  menu.addAction(gmain->_showOnlyCtrlLinksAct);
  menu.addAction(gmain->_showCtrlLinksAct);
  menu.addAction(gmain->_hideCtrlLinksAct);
  menu.addAction(gmain->_emphasisPortLinksAct);
  menu.addAction(gmain->_deEmphasizeAllAct);
  menu.addSeparator();
  menu.addAction(gmain->_zoomToBlocAct);
  menu.addAction(gmain->_centerOnNodeAct);
  menu.addAction(gmain->_shrinkExpand);
//   menu.addSeparator();
//   menu.addAction(gmain->_toggleSceneItemVisibleAct);
  menu.exec(globalPos);
}

//=======================================================================================

InPortMenu::InPortMenu() : MenusBase()
{
}

InPortMenu::~InPortMenu()
{
}

void InPortMenu::popupMenu(QWidget *caller, const QPoint &globalPos, const QString& m)
{
  GenericGui *gmain = QtGuiContext::getQtCurrent()->getGMain();
  bool isEdition = QtGuiContext::getQtCurrent()->isEdition();
  QMenu menu(m, caller);
  addHeader(menu, m);
  menu.addAction(gmain->_showOnlyPortLinksAct);
  menu.addAction(gmain->_showPortLinksAct);
  menu.addAction(gmain->_hidePortLinksAct);
  menu.addAction(gmain->_emphasisPortLinksAct);
  menu.addAction(gmain->_deEmphasizeAllAct);
  if (isEdition)
    {
      Subject* sub = QtGuiContext::getQtCurrent()->getSelectedSubject();
      SubjectDataPort *sdp = dynamic_cast<SubjectDataPort*>(sub);
      Node *parent = 0;
      if (sdp) parent = sdp->getPort()->getNode();
      if (parent)
        if (dynamic_cast<DataNode*>(parent) || dynamic_cast<InlineNode*>(parent))
          {
            menu.addSeparator();
            menu.addAction(gmain->_deleteItemAct);
          }
    }
  menu.exec(globalPos);
}

//=======================================================================================

OutPortMenu::OutPortMenu() : MenusBase()
{
}

OutPortMenu::~OutPortMenu()
{
}

void OutPortMenu::popupMenu(QWidget *caller, const QPoint &globalPos, const QString& m)
{
  GenericGui *gmain = QtGuiContext::getQtCurrent()->getGMain();
  bool isEdition = QtGuiContext::getQtCurrent()->isEdition();
  QMenu menu(m, caller);
  addHeader(menu, m);
  menu.addAction(gmain->_showOnlyPortLinksAct);
  menu.addAction(gmain->_showPortLinksAct);
  menu.addAction(gmain->_hidePortLinksAct);
  menu.addAction(gmain->_emphasisPortLinksAct);
  menu.addAction(gmain->_deEmphasizeAllAct);
  if (isEdition)
    {
      Subject* sub = QtGuiContext::getQtCurrent()->getSelectedSubject();
      SubjectDataPort *sdp = dynamic_cast<SubjectDataPort*>(sub);
      Node *parent = 0;
      if (sdp) parent = sdp->getPort()->getNode();
      if (parent)
        if (dynamic_cast<DataNode*>(parent) || dynamic_cast<InlineNode*>(parent))
          {
            menu.addSeparator();
            menu.addAction(gmain->_deleteItemAct);
          }
    }
  menu.exec(globalPos);
}

//=======================================================================================

CtrlInPortMenu::CtrlInPortMenu() : MenusBase()
{
}

CtrlInPortMenu::~CtrlInPortMenu()
{
}

void CtrlInPortMenu::popupMenu(QWidget *caller, const QPoint &globalPos, const QString& m)
{
  GenericGui *gmain = QtGuiContext::getQtCurrent()->getGMain();
  bool isEdition = QtGuiContext::getQtCurrent()->isEdition();
  QMenu menu(m, caller);
  addHeader(menu, m);
  menu.addAction(gmain->_showOnlyCtrlLinksAct);
  menu.addAction(gmain->_showCtrlLinksAct);
  menu.addAction(gmain->_hideCtrlLinksAct);
  menu.addAction(gmain->_emphasisCtrlLinksAct);
  menu.addAction(gmain->_deEmphasizeAllAct);
  if (isEdition)
    {
      //menu.addSeparator();
      //menu.addAction(gmain->_deleteItemAct);
    }
  menu.exec(globalPos);
}

//=======================================================================================

CtrlOutPortMenu::CtrlOutPortMenu() : MenusBase()
{
}

CtrlOutPortMenu::~CtrlOutPortMenu()
{
}

void CtrlOutPortMenu::popupMenu(QWidget *caller, const QPoint &globalPos, const QString& m)
{
  GenericGui *gmain = QtGuiContext::getQtCurrent()->getGMain();
  bool isEdition = QtGuiContext::getQtCurrent()->isEdition();
  QMenu menu(m, caller);
  addHeader(menu, m);
  menu.addAction(gmain->_showOnlyCtrlLinksAct);
  menu.addAction(gmain->_showCtrlLinksAct);
  menu.addAction(gmain->_hideCtrlLinksAct);
  menu.addAction(gmain->_emphasisCtrlLinksAct);
  menu.addAction(gmain->_deEmphasizeAllAct);
  if (isEdition)
    {
      //menu.addSeparator();
      //menu.addAction(gmain->_deleteItemAct);
    }
  menu.exec(globalPos);
}

//=======================================================================================

ReferenceMenu::ReferenceMenu() : MenusBase()
{
}

ReferenceMenu::~ReferenceMenu()
{
}

void ReferenceMenu::popupMenu(QWidget *caller, const QPoint &globalPos, const QString& m)
{
  GenericGui *gmain = QtGuiContext::getQtCurrent()->getGMain();
  bool isEdition = QtGuiContext::getQtCurrent()->isEdition();
  QMenu menu(m, caller);
  addHeader(menu, m);
  menu.addAction(gmain->_selectReferenceAct);
  menu.exec(globalPos);
}

//=======================================================================================

ContainerDirMenu::ContainerDirMenu() : MenusBase()
{
}

ContainerDirMenu::~ContainerDirMenu()
{
}

void ContainerDirMenu::popupMenu(QWidget *caller, const QPoint &globalPos, const QString& m)
{
  GenericGui *gmain = QtGuiContext::getQtCurrent()->getGMain();
  bool isEdition = QtGuiContext::getQtCurrent()->isEdition();
  QMenu menu(m, caller);
  addHeader(menu, m);
  if (isEdition)
    {
      menu.addAction(gmain->_newContainerAct);
      menu.addAction(gmain->_newHPContainerAct);
    }
  menu.exec(globalPos);
}

//=======================================================================================

ComponentInstanceMenu::ComponentInstanceMenu() : MenusBase()
{
}

ComponentInstanceMenu::~ComponentInstanceMenu()
{
}

void ComponentInstanceMenu::popupMenu(QWidget *caller, const QPoint &globalPos, const QString& m)
{
  GenericGui *gmain = QtGuiContext::getQtCurrent()->getGMain();
  bool isEdition = QtGuiContext::getQtCurrent()->isEdition();
  QMenu menu(m, caller);
  addHeader(menu, m);
  if (isEdition)
    {
      menu.addAction(gmain->_selectComponentInstanceAct);
      menu.addAction(gmain->_newSalomeComponentAct);
    }
  menu.exec(globalPos);
}

//=======================================================================================

ContainerMenu::ContainerMenu() : MenusBase()
{
}

ContainerMenu::~ContainerMenu()
{
}

void ContainerMenu::popupMenu(QWidget *caller, const QPoint &globalPos, const QString& m)
{
  GenericGui *gmain = QtGuiContext::getQtCurrent()->getGMain();
  bool isEdition = QtGuiContext::getQtCurrent()->isEdition();
  QMenu menu(m, caller);
  addHeader(menu, m);
  if (isEdition)
    {
      menu.addAction(gmain->_deleteItemAct);
    }
  menu.exec(globalPos);
}

