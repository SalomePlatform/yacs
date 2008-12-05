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
#include "TablePortsEdition.hxx"
#include "SchemaItem.hxx"
#include "QtGuiContext.hxx"
#include "GuiEditor.hxx"
#include "guiObservers.hxx"
#include "Catalog.hxx"
#include "CatalogWidget.hxx"

#include <QItemSelectionModel>
#include <QDialog>
#include <QInputDialog>
#include <QHeaderView>

#include <cassert>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;
using namespace YACS::HMI;

TablePortsEdition::TablePortsEdition(bool inPorts, QWidget *parent)
{
  DEBTRACE("TablePortsEdition::TablePortsEdition");
  setupUi(this);
  _inPorts = inPorts;
  _node = 0;
  if (tv_ports->horizontalHeader())
      tv_ports->horizontalHeader()->setStretchLastSection(true);
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
}

void TablePortsEdition::on_pb_down_clicked()
{
  DEBTRACE("TablePortsEdition::on_pb_down_clicked");
}

void TablePortsEdition::on_pb_insert_clicked()
{
  DEBTRACE("TablePortsEdition::on_pb_insert_clicked");
}

void TablePortsEdition::on_pb_add_clicked()
{
  DEBTRACE("TablePortsEdition::on_pb_add_clicked");
//   QTabWidget *twPorts = dynamic_cast<QTabWidget*>(parentWidget()->parentWidget());
//   if (twPorts)
//     {
//       int idTab = twPorts->indexOf(this);
//       DEBTRACE(idTab);
//     }
  DEBTRACE(_inPorts);
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

  QStringList types;
  types << "int" << "double" << "string" << "bool";
  bool ok;
  QString selected;

//   QString selected = QInputDialog::getItem(this, "type selection", "port type", types, 0, false, &ok);
//   if (!ok || selected.isEmpty()) return;

  YACS::ENGINE::Catalog *catalog = QtGuiContext::getQtCurrent()->getSessionCatalog();
  if (_inPorts)
    QtGuiContext::getQtCurrent()->getGMain()->_guiEditor->CreateInputPort(_node, "", catalog,
                                                                          selected.toStdString(), spBefore);
  else
    QtGuiContext::getQtCurrent()->getGMain()->_guiEditor->CreateOutputPort(_node, "", catalog,
                                                                           selected.toStdString(), spBefore);
}

void TablePortsEdition::on_cb_insert_activated(const QString& text)
{
  DEBTRACE("TablePortsEdition::on_cb_insert_currentIndexChanged " << text.toStdString());
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
  assert(catalog);
  if (_inPorts)
    QtGuiContext::getQtCurrent()->getGMain()->_guiEditor->CreateInputPort(_node, "", catalog,
                                                                          portType, spBefore);
  else
    QtGuiContext::getQtCurrent()->getGMain()->_guiEditor->CreateOutputPort(_node, "", catalog,
                                                                           portType, spBefore);

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
