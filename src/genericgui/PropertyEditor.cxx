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

#include "PropertyEditor.hxx"

#include <QVBoxLayout>
#include <QToolButton>
#include <QIcon>
#include <QLabel>
#include <QMenu>
#include <QVariant>
#include <QActionGroup>
#include <QHeaderView>
#include <QLineEdit>
#include <QInputDialog>
#include <QMessageBox>
#include "guiObservers.hxx"
#include "Message.hxx"

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace YACS::HMI;
//using namespace YACS::ENGINE;

/*! \class YACS::HMI::PropertyEditor
 *  \brief A class to edit properties of a subject
 *
 */

PropertyEditor::PropertyEditor(Subject* subject,QWidget *parent):QWidget(parent),_subject(subject),_editing(false)
{
  std::vector<std::string> v=_subject->knownProperties();
  for (int i=0; i<v.size(); ++i)
    {
      _knownProperties <<  QString::fromStdString(v[i]);
    }

  _removeAction=new QAction(QIcon("icons:icon_minus.png"),"Remove Property",this);
  _addAction=new QAction(QIcon("icons:icon_plus.png"),"Add Property",this);

  _table=new QTableWidget;
  _table->setColumnCount(2);
  QStringList headers;
  headers << "Name" << "Value";
  _table->setHorizontalHeaderLabels(headers);
  _table->verticalHeader()->hide();
  _table->setSelectionMode(QAbstractItemView::SingleSelection);

  connect(_table, SIGNAL(itemChanged(QTableWidgetItem *)),this, SLOT(onItemChanged(QTableWidgetItem *)));
  connect(_removeAction, SIGNAL(triggered()), this, SLOT(onRemoveProperty()));

  QHBoxLayout* hboxLayout = new QHBoxLayout();
  hboxLayout->setMargin(0);
  QToolButton* tb_options = new QToolButton();
  tb_options->setCheckable(true);
  QIcon icon;
  icon.addFile("icons:icon_down.png");
  icon.addFile("icons:icon_up.png", QSize(), QIcon::Normal, QIcon::On);
  tb_options->setIcon(icon);
  hboxLayout->addWidget(tb_options);
  connect(tb_options, SIGNAL(toggled(bool)), this, SLOT(on_tb_options_toggled(bool)));

  QLabel* label=new QLabel("Properties     ");
  QFont font;
  font.setBold(true);
  font.setWeight(75);
  label->setFont(font);
  hboxLayout->addWidget(label);

  _bar=new QToolBar();

  QToolButton* button=new QToolButton();
  button->setDefaultAction(_addAction);
  button->setPopupMode(QToolButton::InstantPopup);
  _bar->addWidget(button);

  button=new QToolButton();
  button->setDefaultAction(_removeAction);
  _bar->addWidget(button);
  hboxLayout->addWidget(_bar);

  QVBoxLayout *layout = new QVBoxLayout;
  layout->setMargin(0);
  layout->addLayout(hboxLayout);
  layout->addWidget(_table);
  setLayout(layout);

  _table->hide();
  _bar->hide();

  update();
  updateMenu();

  _editing=true;
}

PropertyEditor::~PropertyEditor()
{
}

//! Qt slot to remove the selected item in the table
/*!
 */
void PropertyEditor::onRemoveProperty()
{
  DEBTRACE("PropertyEditor::onRemoveProperty");
  QList<QTableWidgetItem *> litems=_table->selectedItems();
  if(litems.isEmpty())
    return;
  QTableWidgetItem *item =litems.first();
  _propertyNames.removeOne(item->text());
  _table->removeRow(_table->row(item));
  updateMenu();
  setProperties();
}

//! Qt slot to add a property item in the table
/*!
 * \param action : the triggered action
 */
void PropertyEditor::onAddProperty(QAction* action)
{
  DEBTRACE("PropertyEditor::onAddProperty " << action->text().toStdString());
  bool ok;
  QString text=action->text();
  if(text == "Other ...")
    {
      text = QInputDialog::getText(this, tr("New Property"),tr("Name:"), QLineEdit::Normal, "", &ok);
      if (!ok || text.isEmpty())
        return;
    }

  if(_propertyNames.contains(text))
    {
      QMessageBox::warning ( 0, "Property already defined", "Property already defined");
      return;
    }

  _editing=false;
  int row=_table->rowCount();
  _table->setRowCount(row+1);

  QTableWidgetItem *newItem = new QTableWidgetItem(text);
  newItem->setFlags(newItem->flags() & ~Qt::ItemIsEditable);
  _table->setItem(row,0, newItem);

  newItem = new QTableWidgetItem();
  _table->setItem(row,1, newItem);
  _propertyNames << text;
  _editing=true;
  updateMenu();
}

//! Qt slot to change the value of a property item in the table
/*!
 * \param item : the item changed
 */
void PropertyEditor::onItemChanged(QTableWidgetItem * item)
{
  DEBTRACE("PropertyEditor::onItemChanged " << _editing);
  if(!_editing)
    return;
  setProperties();
}

//! Update the property items with their values in the subject
/*!
 */
void PropertyEditor::update()
{
  DEBTRACE("PropertyEditor::update " );
  _editing=false;
  QTableWidgetItem *newItem;
  int row=0;
  _table->setRowCount(0);
  _propertyNames << QStringList();
  std::map<std::string,std::string> props=_subject->getProperties();
  for (std::map<std::string, std::string>::iterator it = props.begin(); it != props.end(); ++it)
    {
      _table->setRowCount(row+1);
      QTableWidgetItem *newItem = new QTableWidgetItem(it->first.c_str());
      newItem->setFlags(newItem->flags() & ~Qt::ItemIsEditable);
      _table->setItem(row,0, newItem);

      newItem = new QTableWidgetItem(it->second.c_str());
      _table->setItem(row,1, newItem);
      _propertyNames << it->first.c_str();
      row=row+1;
    }
  _editing=true;
}

//! Update the menu used to add properties with the already set properties
/*!
 */
void PropertyEditor::updateMenu()
{
  DEBTRACE("PropertyEditor::updateMenu " );
  QMenu* menu=new QMenu;
  _addAction->setMenu(menu);

  QActionGroup* actGroup=new QActionGroup(this);
  connect(actGroup, SIGNAL(triggered(QAction*)), this, SLOT(onAddProperty(QAction*)));

  QAction* anAction;
  for (int i = 0; i < _knownProperties.size(); ++i)
    {
      if(_propertyNames.contains(_knownProperties.at(i)))
        continue;
      anAction= actGroup->addAction(_knownProperties.at(i)); 
      anAction->setData(QVariant(_knownProperties.at(i)));
      menu->addAction(anAction);
    }
  anAction= actGroup->addAction("Other ..."); 
  menu->addAction(anAction);
  anAction->setData(QVariant("..."));
}

//! Update the subject properties with the values stored in the table
/*!
 */
void PropertyEditor::setProperties()
{
  DEBTRACE("PropertyEditor::setProperties " );
  QTableWidgetItem *item;
  std::string name;
  std::string value;
  std::map<std::string,std::string> props;
  for (int i = 0; i < _table->rowCount(); ++i)
    {
      item=_table->item(i,0);
      name=item->data(Qt::DisplayRole).toString().toStdString();
      item=_table->item(i,1);
      value=item->data(Qt::DisplayRole).toString().toStdString();
      props[name]=value;
    }
  bool ret=_subject->setProperties(props);
  if(!ret)
     Message mess;
}

void PropertyEditor::on_tb_options_toggled(bool checked)
{
  DEBTRACE("PropertyEditor::on_tb_options_toggled " << checked);
  if(checked)
    {
      _table->show();
      _bar->show();
    }
  else
    {
      _table->hide();
      _bar->hide();
    }
}

