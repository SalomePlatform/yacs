
#include "EditionSwitch.hxx"
#include "TableSwitch.hxx"
#include "SchemaModel.hxx"
#include "QtGuiContext.hxx"
#include "SchemaItem.hxx"

#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;

using namespace YACS;
using namespace YACS::HMI;

EditionSwitch::EditionSwitch(Subject* subject,
                             QWidget* parent,
                             const char* name)
  : EditionBloc(subject, parent, name)
{
  _tvSwitch = new TableSwitch(this);
  _wid->gridLayout1->addWidget(_tvSwitch);
  SchemaModel *model = QtGuiContext::getQtCurrent()->getSchemaModel();
  _tvSwitch->tv_nodes->setModel(model);
}

EditionSwitch::~EditionSwitch()
{
}

void EditionSwitch::synchronize()
{
  SchemaModel *model = QtGuiContext::getQtCurrent()->getSchemaModel();
  SchemaItem* schemaItem = QtGuiContext::getQtCurrent()->_mapOfSchemaItem[_subject];
  if (schemaItem)
    {
      QModelIndex parentIndex = schemaItem->modelIndex();
      _tvSwitch->tv_nodes->setRootIndex(parentIndex);
      int numRows = model->rowCount(parentIndex);
      for (int row = 0; row < numRows; ++row)
        {
          QModelIndex index = model->index(row, 0, parentIndex);
          SchemaItem *childItem = static_cast<SchemaItem*>(index.internalPointer());
          bool hidden = true;
          if (Subject *sub = childItem->getSubject())
            if (dynamic_cast<SubjectNode*>(sub))
              hidden = false;
          _tvSwitch->tv_nodes->setRowHidden(row, hidden);
        }
    }
}

