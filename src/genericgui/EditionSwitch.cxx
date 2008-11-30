
#include "EditionSwitch.hxx"
#include "TableSwitch.hxx"
#include "SchemaModel.hxx"
#include "QtGuiContext.hxx"
#include "SchemaItem.hxx"
#include "ValueDelegate.hxx"

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

#include <cassert>
#include <sstream>

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
  _tvSwitch->sb_select->setMinimum(INT_MIN);
  _tvSwitch->sb_select->setMaximum(INT_MAX);
  connect(_tvSwitch->sb_select, SIGNAL(valueChanged(const QString &)),
          this, SLOT(onModifySelect(const QString &)));

  _valueDelegate = new ValueDelegate(parent);
  _tvSwitch->tv_nodes->setItemDelegateForColumn(YValue, _valueDelegate);
  _tvSwitch->setEditableCase(true);

  connect(_valueDelegate, SIGNAL(commitData(QWidget*)),
          this, SLOT(onCommitData(QWidget*)));
}

EditionSwitch::~EditionSwitch()
{
}

void EditionSwitch::synchronize()
{
  DEBTRACE("EditionSwitch::synchronize");
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
  _tvSwitch->adjustColumns();
  _subject->update(SETSELECT, 0, _subject);
}

void EditionSwitch::onModifySelect(const QString &text)
{
  DEBTRACE("EditionSwitch::onModifySelect " << text.toStdString());
  SubjectSwitch *sswitch = dynamic_cast<SubjectSwitch*>(_subject);
  assert(sswitch);
  sswitch->setSelect(text.toStdString());
}

void EditionSwitch::update(GuiEvent event, int type, Subject* son)
{
  DEBTRACE("EditionSwitch::update " <<eventName(event) << " " << type);
  switch (event)
    {

    case SETSELECT:
      {
        SubjectComposedNode *scn = dynamic_cast<SubjectComposedNode*>(_subject);
        string val = scn->getValue();
        istringstream ss(val);
        DEBTRACE(val);
        int i = 0;
        ss >> i;
        DEBTRACE(i);
        _tvSwitch->sb_select->setValue(i);
      }
      break;
    default:
      EditionBloc::update(event, type, son);
    }
}

void EditionSwitch::onCommitData(QWidget *editor)
{
  DEBTRACE("EditionSwitch::onCommitData " << editor);
  GenericEditor* gedit = dynamic_cast<GenericEditor*>(editor);
  assert(gedit);
  QString val = gedit->GetStrValue();
  DEBTRACE(val.toStdString());
  Subject *sub = gedit->getSubject();
  assert(sub);
  SubjectNode *snode = dynamic_cast<SubjectNode*>(sub);
  assert(snode);
  sub = snode->getParent();
  SubjectSwitch *sswitch = dynamic_cast<SubjectSwitch*>(sub);
  assert(sswitch);
  bool isOk = sswitch->setCase(val.toStdString(), snode);
  if (_valueDelegate)
    _valueDelegate->setResultEditing(editor, isOk);

}
