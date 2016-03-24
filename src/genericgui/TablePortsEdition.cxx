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

#include "TablePortsEdition.hxx"
#include "SchemaItem.hxx"
#include "QtGuiContext.hxx"
#include "GuiEditor.hxx"
#include "guiObservers.hxx"
#include "Catalog.hxx"
#include "CatalogWidget.hxx"
#include "DataPort.hxx"
#include "InputPort.hxx"
#include "OutputPort.hxx"
#include "ElementaryNode.hxx"
#include "Scene.hxx"
#include "SceneItem.hxx"
#include "SceneComposedNodeItem.hxx"

#include <QItemSelectionModel>
#include <QDialog>
#include <QInputDialog>
#include <QHeaderView>

#include <cassert>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;
using namespace YACS::HMI;
using namespace YACS::ENGINE;

YComboBox::YComboBox(QWidget *parent)
  : QComboBox(parent)
{
  DEBTRACE("ComboBox::ComboBox");
}

YComboBox::~YComboBox()
{
}

void YComboBox::hidePopup()
{
  DEBTRACE("YComboBox::hidePopup");
  QComboBox::hidePopup();
  emit popupHide();
}

void YComboBox::showPopup()
{
  DEBTRACE("YComboBox::showPopup");
  QComboBox::showPopup();
  emit popupShow();
}

void YComboBox::keyPressEvent(QKeyEvent *e)
{
  //accept all key events but do nothing to avoid creating ports with keys
}

#ifndef QT_NO_WHEELEVENT
void YComboBox::wheelEvent(QWheelEvent *e)
{
  //idem
}
#endif


TablePortsEdition::TablePortsEdition(bool inPorts, QWidget *parent)
{
  DEBTRACE("TablePortsEdition::TablePortsEdition");
  setupUi(this);
  _inPorts = inPorts;
  _node = 0;
  _nbUp = 0;
  if (tv_ports->horizontalHeader())
      tv_ports->horizontalHeader()->setStretchLastSection(true);

  delete cb_insert;
  cb_insert = new YComboBox(layoutWidget);
  gridLayout1->addWidget(cb_insert, 0, 4, 1, 1);
  cb_insert->setToolTip("port creation: select a port type");

  connect(cb_insert, SIGNAL(popupHide()),
          this, SLOT(oncb_insert_popupHide()));
  connect(cb_insert, SIGNAL(popupShow()),
          this, SLOT(oncb_insert_popupShow()));

  connect(cb_insert, SIGNAL(activated(const QString&)),
          this, SLOT(oncb_insert_activated(const QString&)));
}

TablePortsEdition::~TablePortsEdition()
{
  DEBTRACE("TablePortsEdition::~TablePortsEdition");
}

void TablePortsEdition::setNode(SubjectElementaryNode* node)
{
  _node = node;
}

void TablePortsEdition::on_pb_up_clicked()
{
  DEBTRACE("TablePortsEdition::on_pb_up_clicked");
  upOrDown(1);// --- n>0 means go up n steps
  _nbUp = 0;
}

void TablePortsEdition::on_pb_down_clicked()
{
  DEBTRACE("TablePortsEdition::on_pb_down_clicked");
  upOrDown(0); // --- 0 means go down one step
  _nbUp = 0;
}

/*! move up or down the port in the list of ports.
 *  when isUp = 0, go down one step, when isUp = n>0,
 *  go up n steps.
 */
void TablePortsEdition::upOrDown(int isUp)
{
  SchemaModel *model = QtGuiContext::getQtCurrent()->getSchemaModel();
  SubjectDataPort *spToMove = 0;

  QModelIndexList items = tv_ports->selectionModel()->selection().indexes();
  QModelIndex index;
  if (!items.isEmpty()) index = items.first();
  SchemaItem *item = static_cast<SchemaItem*>(index.internalPointer());

  if (item)
    {
      Subject *sub = item->getSubject();
      DEBTRACE(sub->getName());
      spToMove = dynamic_cast<SubjectDataPort*>(sub);
    }

  if (spToMove)
    {
      Subject *sub = item->parent()->getSubject();
      SubjectElementaryNode* sen = dynamic_cast<SubjectElementaryNode*>(sub);
      YASSERT(sen);
      sen->OrderDataPorts(spToMove,isUp);
      if (Scene::_autoComputeLinks)
        {
          YACS::HMI::SubjectProc* subproc = QtGuiContext::getQtCurrent()->getSubjectProc();
          SceneItem *item = QtGuiContext::getQtCurrent()->_mapOfSceneItem[subproc];
          SceneComposedNodeItem *proc = dynamic_cast<SceneComposedNodeItem*>(item);
          proc->rebuildLinks();
        }

      QItemSelectionModel *selectionModel = tv_ports->selectionModel();
      QModelIndex topLeft=item->modelIndex(0);
      QItemSelection selection(topLeft, topLeft);
      selectionModel->select(selection, QItemSelectionModel::Select);
    }
}

void TablePortsEdition::on_pb_insert_clicked()
{
  DEBTRACE("TablePortsEdition::on_pb_insert_clicked");
  SchemaModel *model = QtGuiContext::getQtCurrent()->getSchemaModel();
  SubjectDataPort *spBefore = 0;
  _nbUp = 0;

  QModelIndexList items = tv_ports->selectionModel()->selection().indexes();
  QModelIndex index;
  if (!items.isEmpty()) index = items.first();
  SchemaItem *item = static_cast<SchemaItem*>(index.internalPointer());
  if (!item)
    return;

  Subject *sub = item->getSubject();
  DEBTRACE(sub->getName());
  spBefore = dynamic_cast<SubjectDataPort*>(sub);
  DataPort *port = spBefore->getPort();

  bool isInput = false;
  if (dynamic_cast<SubjectInputPort*>(spBefore))
    isInput = true;

  sub = item->parent()->getSubject();
  SubjectElementaryNode* sen = dynamic_cast<SubjectElementaryNode*>(sub);
  YASSERT(sen);
  ElementaryNode* father = dynamic_cast<ElementaryNode*>(sen->getNode());

  if (isInput)
    {
      int rang = 0;
      list<InputPort*> plist = father->getSetOfInputPort();
      list<InputPort*>::iterator pos = plist.begin();
      for (; (*pos) != port; pos++)
        rang++;
      _nbUp = plist.size() -rang;
    }
  else
    {
      int rang = 0;
      list<OutputPort*> plist = father->getSetOfOutputPort();
      list<OutputPort*>::iterator pos = plist.begin();
      for (; (*pos) != port; pos++)
        rang++;
      _nbUp = plist.size() -rang;
    }

  DEBTRACE(_nbUp);
  cb_insert->showPopup();  
}

void TablePortsEdition::oncb_insert_activated(const QString& text)
{
  DEBTRACE("TablePortsEdition::oncb_insert_activated " << text.toStdString());
  SubjectDataPort *spBefore = 0;
  QModelIndexList items = tv_ports->selectionModel()->selection().indexes();
  QModelIndex index;
  if (!items.isEmpty()) index = items.first();
  SchemaItem *item = static_cast<SchemaItem*>(index.internalPointer());
  if (item)
    {
      Subject *sub = item->getSubject();
      DEBTRACE(sub->getName());
      spBefore = dynamic_cast<SubjectDataPort*>(sub);
    }
  string portType = text.toStdString();
  QStringList defaultTypes;
  defaultTypes << "Int" << "Double" << "String" << "Bool";
  if (defaultTypes.contains(text))
    portType = text.toLower().toStdString();
  YACS::ENGINE::Catalog *catalog =
    QtGuiContext::getQtCurrent()->getGMain()->getCatalogWidget()->getCatalogFromType(portType);
  if (!catalog) catalog = QtGuiContext::getQtCurrent()->getCurrentCatalog();
  YASSERT(catalog);
  GuiEditor *editor = QtGuiContext::getQtCurrent()->getGMain()->_guiEditor;
  SubjectDataPort * sdp = 0;
  if (_inPorts)
    sdp = editor->CreateInputPort(_node, "", catalog, portType, spBefore);
  else
    sdp = editor->CreateOutputPort(_node, "", catalog, portType, spBefore);
  if (_nbUp)
    {
      DEBTRACE("move up created port " << _nbUp);
      _node->OrderDataPorts(sdp, _nbUp);
      _nbUp = 0;
    }
}

void TablePortsEdition::on_pb_remove_clicked()
{
  DEBTRACE("TablePortsEdition::on_pb_remove_clicked");
  SchemaModel *model = QtGuiContext::getQtCurrent()->getSchemaModel();
  SubjectDataPort *spToRemove = 0;
  QModelIndexList items = tv_ports->selectionModel()->selection().indexes();
  QModelIndex index;
  if (!items.isEmpty()) index = items.first();
  SchemaItem *item = static_cast<SchemaItem*>(index.internalPointer());
  if (item)
    {
      Subject *sub = item->getSubject();
      DEBTRACE(sub->getName());
      spToRemove = dynamic_cast<SubjectDataPort*>(sub);
    }
  if (spToRemove)
    {
      QtGuiContext::getQtCurrent()->getGMain()->_guiEditor->DeleteSubject(spToRemove->getParent(),
                                                                          spToRemove);
    }
}

void TablePortsEdition::adjustColumns()
{
  DEBTRACE("TablePortsEdition::adjustColumns");
  if (!tv_ports->isColumnHidden(YLabel))
    tv_ports->resizeColumnToContents(YLabel); 
  if (!tv_ports->isColumnHidden(YType))
    tv_ports->resizeColumnToContents(YType); 
}


void TablePortsEdition::setEditablePorts(bool isEditable)
{
  DEBTRACE("TablePortsEdition::setEditablePorts " << isEditable);
  if (QtGuiContext::getQtCurrent()->isEdition())
    {
      tv_ports->setColumnHidden(YType,  false);
      tv_ports->setColumnHidden(YValue, false);
    }
  else
    {
      isEditable = false; // --- no edition in execution !
      tv_ports->setColumnHidden(YType,  true);
      tv_ports->setColumnHidden(YState, false);
    }

  if (tv_ports->verticalHeader())
    tv_ports->verticalHeader()->setVisible(false);


  pb_up->setEnabled(isEditable);
  pb_down->setEnabled(isEditable);
  pb_insert->setEnabled(isEditable);
  pb_remove->setEnabled(isEditable);
  cb_insert->setEnabled(isEditable);
  if (isEditable)
    {
      pb_up->show();
      pb_down->show();
      pb_insert->show();
      pb_remove->show();
      cb_insert->show();
    }
  else
    {
      pb_up->hide();
      pb_down->hide();
      pb_insert->hide();
      pb_remove->hide();
      cb_insert->hide();
    }
}

void TablePortsEdition::oncb_insert_popupHide()
{
  DEBTRACE("TablePortsEdition::oncb_insert_popupHide");
  if (cb_insert->currentIndex() < 0) 
    _nbUp = 0; // --- no selection, no port creation, no move
  DEBTRACE(_nbUp);
}

void TablePortsEdition::oncb_insert_popupShow()
{
  DEBTRACE("TablePortsEdition::oncb_insert_popupShow");
}

