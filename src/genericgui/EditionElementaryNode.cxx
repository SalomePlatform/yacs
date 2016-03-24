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

#include "EditionElementaryNode.hxx"
#include "ValueDelegate.hxx"
#include "QtGuiContext.hxx"

#include "Proc.hxx"
#include "Port.hxx"
#include "DataPort.hxx"
#include "TypeCode.hxx"

#include "TablePortsEdition.hxx"

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

#include <cassert>

using namespace std;

using namespace YACS;
using namespace YACS::HMI;

EditionElementaryNode::EditionElementaryNode(Subject* subject,
                                             QWidget* parent,
                                             const char* name)
  : EditionNode(subject, parent, name)
{
  _subElemNode = 0;
  _twPorts = 0;
  _tvInPorts = 0;
  _tvOutPorts = 0;
  _valueDelegate = 0;

  _subElemNode = dynamic_cast<SubjectElementaryNode*>(_subject);
  YASSERT(_subElemNode);
  _valueDelegate = new ValueDelegate(parent);

  connect(_valueDelegate, SIGNAL(commitData(QWidget*)),
          this, SLOT(onCommitData(QWidget*)));

}

EditionElementaryNode::~EditionElementaryNode()
{
}

void EditionElementaryNode::onApply()
{
  ItemEdition::onApply();
}

void EditionElementaryNode::onCancel()
{
  ItemEdition::onCancel();
}

void EditionElementaryNode::onPortIndexChanged(int index)
{
  DEBTRACE("EditionElementaryNode::onPortIndexChanged " << index);
  synchronize();
}

void EditionElementaryNode::synchronize()
{
  //DEBTRACE("EditionElementaryNode::synchronize");
  SchemaModel *model = QtGuiContext::getQtCurrent()->getSchemaModel();
  SchemaItem* schemaItem = QtGuiContext::getQtCurrent()->_mapOfSchemaItem[_subject];
  if (schemaItem)
    {
      QModelIndex parentIndex = schemaItem->modelIndex();
      int numRows = model->rowCount(parentIndex);
      if (_tvInPorts)
        {
          _tvInPorts->cb_insert->setCurrentIndex(0);
          _tvInPorts->setNode(_subElemNode);
          _tvInPorts->tv_ports->setRootIndex(parentIndex);
          for (int row = 0; row < numRows; ++row)
            {
              QModelIndex index = model->index(row, 0, parentIndex);
              SchemaItem *childItem = static_cast<SchemaItem*>(index.internalPointer());
              bool hidden = true;
              if (Subject *sub = childItem->getSubject())
                if (dynamic_cast<SubjectInputPort*>(sub)
                    || dynamic_cast<SubjectInputDataStreamPort*>(sub))
                  hidden = false;
              _tvInPorts->tv_ports->setRowHidden(row, hidden);
            }
          _tvInPorts->adjustColumns();
        }
      if (_tvOutPorts)
        {
          _tvOutPorts->cb_insert->setCurrentIndex(0);
          _tvOutPorts->setNode(_subElemNode);
          _tvOutPorts->tv_ports->setRootIndex(parentIndex);
          for (int row = 0; row < numRows; ++row)
            {
              QModelIndex index = model->index(row, 0, parentIndex);
              SchemaItem *childItem = static_cast<SchemaItem*>(index.internalPointer());
              bool hidden = true;
              if (Subject *sub = childItem->getSubject())
                if (dynamic_cast<SubjectOutputPort*>(sub)
                    || dynamic_cast<SubjectOutputDataStreamPort*>(sub))
                  hidden = false;
              _tvOutPorts->tv_ports->setRowHidden(row, hidden);
            } 
          _tvOutPorts->adjustColumns();
        }
    }
}

void EditionElementaryNode::setEditablePorts(bool isEditable)
{
  if (_tvInPorts) _tvInPorts->setEditablePorts(isEditable);
  if (_tvOutPorts) _tvOutPorts->setEditablePorts(isEditable);
}

bool EditionElementaryNode::hasInputPorts()
{
  return true;
}

bool EditionElementaryNode::hasOutputPorts()
{
  return true;
}

void EditionElementaryNode::createTablePorts(QLayout* layout)
{
  _twPorts = new QTabWidget(this);
  layout->addWidget(_twPorts);
  SchemaModel *model = QtGuiContext::getQtCurrent()->getSchemaModel();

  QModelIndex schemIndex = model->index(0, 0, QModelIndex());
  QModelIndex TypesDirIndex = model->index(0, 0, schemIndex);
  
  if (hasInputPorts())
    {
      _tvInPorts =new TablePortsEdition(true, _twPorts);
      _tvInPorts->tv_ports->setModel(model);
      _tvInPorts->tv_ports->setItemDelegateForColumn(YLabel, _valueDelegate); // --- port label
      _tvInPorts->tv_ports->setItemDelegateForColumn(YValue, _valueDelegate); // --- port value
      _tvInPorts->cb_insert->setModel(model);
      _tvInPorts->cb_insert->setRootModelIndex(TypesDirIndex);
      _twPorts->addTab(_tvInPorts, "input Ports");
    }

  if (hasOutputPorts())
    {
      _tvOutPorts =new TablePortsEdition(false, _twPorts);
      _tvOutPorts->tv_ports->setModel(model);
      _tvOutPorts->tv_ports->setItemDelegateForColumn(YLabel, _valueDelegate); // --- port label
      _tvOutPorts->tv_ports->setItemDelegateForColumn(YValue, _valueDelegate); // --- port value
      _tvOutPorts->cb_insert->setModel(model);
      _tvOutPorts->cb_insert->setRootModelIndex(TypesDirIndex);
      _twPorts->addTab(_tvOutPorts, "output Ports");
    }

  connect(_twPorts, SIGNAL(currentChanged(int)),
          this, SLOT(onPortIndexChanged(int)));

  setEditablePorts(false);
}

/*!
 *  After edition with a specific editor created by ValueDelegate
 *  for a cell of TablePortsEdition, the resulting string is tested
 *  for setValue on the corresponding subject. Result of the setValue
 *  (succes or failure) is transmitted to ValueDelegate for further
 *  action in case of failure.
 */
void EditionElementaryNode::onCommitData(QWidget *editor)
{
  DEBTRACE("EditionElementaryNode::onCommitData " << editor);
  GenericEditor* gedit = dynamic_cast<GenericEditor*>(editor);
  YASSERT(gedit);
  QString val = gedit->GetStrValue();
  DEBTRACE(val.toStdString());
  Subject *sub = gedit->getSubject();
  YASSERT(sub);
  SubjectDataPort *sdp = dynamic_cast<SubjectDataPort*>(sub);
  YASSERT(sdp);
  string strval = val.toStdString();
  bool isOk = false;

  if (gedit->getColumnInSubject() == YValue)
    {
      DEBTRACE(strval);
       isOk = sdp->setValue(strval);

      GuiExecutor* executor = QtGuiContext::getQtCurrent()->getGuiExecutor();
      if (executor) executor->setInPortValue(sdp->getPort(), strval);
    }

  else // --- YLabel
    {
      isOk = sdp->setName(strval);
    }

  if (_valueDelegate)
    _valueDelegate->setResultEditing(editor, isOk);
 }
