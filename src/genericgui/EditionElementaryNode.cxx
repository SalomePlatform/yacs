
#include "EditionElementaryNode.hxx"
#include "ValueDelegate.hxx"
#include "QtGuiContext.hxx"

#include "Port.hxx"
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
  assert(_subElemNode);
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

void EditionElementaryNode::createTablePorts()
{
  _twPorts = new QTabWidget(this);
  _wid->gridLayout1->addWidget(_twPorts);
  SchemaModel *model = QtGuiContext::getQtCurrent()->getSchemaModel();

  QModelIndex schemIndex = model->index(0, 0, QModelIndex());
  QModelIndex TypesDirIndex = model->index(0, 0, schemIndex);
  
  if (hasInputPorts())
    {
      _tvInPorts =new TablePortsEdition(true, _twPorts);
      _tvInPorts->tv_ports->setModel(model);
      _tvInPorts->tv_ports->setItemDelegateForColumn(YValue, _valueDelegate); // --- port value
      _tvInPorts->cb_insert->setModel(model);
      _tvInPorts->cb_insert->setRootModelIndex(TypesDirIndex);
      _twPorts->addTab(_tvInPorts, "input Ports");
    }

  if (hasOutputPorts())
    {
      _tvOutPorts =new TablePortsEdition(false, _twPorts);
      _tvOutPorts->tv_ports->setModel(model);
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
  assert(gedit);
  QString val = gedit->GetStrValue();
  DEBTRACE(val.toStdString());
  Subject *sub = gedit->getSubject();
  assert(sub);
  SubjectDataPort *sdp = dynamic_cast<SubjectDataPort*>(sub);
  assert(sdp);
  bool isOk = sdp->setValue(val.toStdString());
  if (_valueDelegate)
    _valueDelegate->setResultEditing(editor, isOk);
 }
