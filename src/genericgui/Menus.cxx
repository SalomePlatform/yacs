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
#include "Menus.hxx"
#include "QtGuiContext.hxx"
#include "guiObservers.hxx"
#include "DataPort.hxx"
#include "DataNode.hxx"
#include "InlineNode.hxx"

#include <QMenu>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;
using namespace YACS::HMI;
using namespace YACS::ENGINE;

//=======================================================================================

MenusBase::MenusBase()
{
  _dummyAct = new QAction(tr("No Action"), this);
//   _dummyAct->setShortcut(tr("Ctrl+y"));
  _dummyAct->setStatusTip(tr("This is just the title"));
  _dummyAct->setToolTip(tr("the menu title"));
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
      CNmenu->addAction(gmain->_FOREACHNodeAct);
      CNmenu->addAction(gmain->_WHILENodeAct);
      CNmenu->addAction(gmain->_SWITCHNodeAct);
      menu.addSeparator();
    }
  menu.addAction(gmain->_toggleSceneItemVisibleAct);
  menu.addAction(gmain->_arrangeLocalNodesAct);
  menu.addAction(gmain->_arrangeRecurseNodesAct);
  menu.addSeparator();
  menu.addAction(gmain->_computeLinkAct);
  menu.addAction(gmain->_toggleAutomaticComputeLinkAct);
  menu.addAction(gmain->_toggleSimplifyLinkAct);
  menu.addAction(gmain->_toggleForce2NodesLinkAct);
  if (isEdition)
    {
      menu.addSeparator();
      menu.addAction(gmain->_deleteItemAct);
      menu.addAction(gmain->_cutItemAct);
      menu.addAction(gmain->_copyItemAct);
      menu.addAction(gmain->_pasteItemAct);
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
  if (isEdition)
    {
      menu.addAction(gmain->_runLoadedSchemaAct);
      menu.addSeparator();
      menu.addAction(gmain->_importDataTypeAct);
      menu.addAction(gmain->_newContainerAct);
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
      CNmenu->addAction(gmain->_FOREACHNodeAct);
      CNmenu->addAction(gmain->_WHILENodeAct);
      CNmenu->addAction(gmain->_SWITCHNodeAct);
      menu.addSeparator();
      menu.addAction(gmain->_pasteItemAct);
      menu.addSeparator();
    }
  menu.addAction(gmain->_getYacsContainerLogAct);
  menu.addSeparator();
  menu.addAction(gmain->_toggleSceneItemVisibleAct);
  menu.addAction(gmain->_arrangeLocalNodesAct);
  menu.addAction(gmain->_arrangeRecurseNodesAct);
  menu.addSeparator();
  menu.addAction(gmain->_computeLinkAct);
  menu.addAction(gmain->_toggleAutomaticComputeLinkAct);
  menu.addAction(gmain->_toggleSimplifyLinkAct);
  menu.addAction(gmain->_toggleForce2NodesLinkAct);
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
  if (isEdition)
    {
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
      menu.addAction(gmain->_deleteItemAct);
      menu.addAction(gmain->_cutItemAct);
      menu.addAction(gmain->_copyItemAct);
      menu.addAction(gmain->_pasteItemAct);
    }
  else
    {
      menu.addAction(gmain->_getErrorReportAct);
      menu.addAction(gmain->_getErrorDetailsAct);
      menu.addAction(gmain->_getContainerLogAct);
    }
  menu.addSeparator();
  menu.addAction(gmain->_toggleSceneItemVisibleAct);
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
  if (isEdition)
    {
      Subject* sub = QtGuiContext::getQtCurrent()->getSelectedSubject();
      SubjectDataPort *sdp = dynamic_cast<SubjectDataPort*>(sub);
      Node *parent = 0;
      if (sdp) parent = sdp->getPort()->getNode();
      if (parent)
        if (dynamic_cast<DataNode*>(parent) || dynamic_cast<InlineNode*>(parent))
          menu.addAction(gmain->_deleteItemAct);
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
  if (isEdition)
    {
      Subject* sub = QtGuiContext::getQtCurrent()->getSelectedSubject();
      SubjectDataPort *sdp = dynamic_cast<SubjectDataPort*>(sub);
      Node *parent = 0;
      if (sdp) parent = sdp->getPort()->getNode();
      if (parent)
        if (dynamic_cast<DataNode*>(parent) || dynamic_cast<InlineNode*>(parent))
          menu.addAction(gmain->_deleteItemAct);
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
  if (isEdition)
    {
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
  if (isEdition)
    {
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

