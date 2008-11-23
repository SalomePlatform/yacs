#include "SchemaNodeItem.hxx"
#include "SchemaInPortItem.hxx"
#include "SchemaOutPortItem.hxx"
#include "ItemMimeData.hxx"
#include "QtGuiContext.hxx"
#include "GuiExecutor.hxx"

//#include "commandsProc.hxx"
#include "Node.hxx"
#include "Menus.hxx"

#include <QIcon>

#include <cassert>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;
using namespace YACS::ENGINE;
using namespace YACS::HMI;

SchemaNodeItem::SchemaNodeItem(SchemaItem *parent, QString label, Subject* subject)
  : SchemaItem(parent, label, subject)
{
  DEBTRACE("SchemaNodeItem::SchemaNodeItem");
  _itemDeco.replace(YLabel, QIcon("icons:node.png"));
  SchemaModel *model = QtGuiContext::getQtCurrent()->getSchemaModel();
  if (!model->isEdition())
    {
      _itemCheckState.replace(YLabel, Qt::Unchecked);
      setExecState(YACS::UNDEFINED);
    }
}

SchemaNodeItem::~SchemaNodeItem()
{
  DEBTRACE("SchemaNodeItem::~SchemaNodeItem");
}

void SchemaNodeItem::update(GuiEvent event, int type, Subject* son)
{
  DEBTRACE("SchemaNodeItem::update "<<event<<" "<<type<<" "<<son);
  SchemaModel *model = QtGuiContext::getQtCurrent()->getSchemaModel();
  SchemaItem *item = 0;
  SubjectNode *snode = 0;
  Node* node = 0;
  switch (event)
    {
    case YACS::HMI::ADD:
      switch (type)
        {
        case YACS::HMI::INPUTPORT:
        case YACS::HMI::INPUTDATASTREAMPORT:
          {
            int nbsons = childCount();
            model->beginInsertRows(modelIndex(), nbsons, nbsons);
            item =  new SchemaInPortItem(this,
                                         son->getName().c_str(),
                                         son);
            model->endInsertRows();
          }
          break;
        case YACS::HMI::OUTPUTPORT:
        case YACS::HMI::OUTPUTDATASTREAMPORT:
          {
            int nbsons = childCount();
            model->beginInsertRows(modelIndex(), nbsons, nbsons);
            item =  new SchemaOutPortItem(this,
                                          son->getName().c_str(),
                                          son);
            model->endInsertRows();
          }
          break;
        default:
          DEBTRACE("SchemaNodeItem::update(), ADD, type not handled: " << type);
        }
      break;
    case YACS::HMI::UPDATE:
      snode = dynamic_cast<SubjectNode*>(_subject);
      assert(snode);
      node = snode->getNode();
      assert(node);
      switch (node->getState())
        {
        case YACS::INVALID:
          _itemForeground.replace(YLabel, QColor("red"));
          model->setData(modelIndex(), 0);  // --- to emit dataChanged signal
          break;
        case YACS::READY:
          _itemForeground.replace(YLabel, QColor("blue"));
          model->setData(modelIndex(), 0);
          break;
        default:
          break;
        }
      break;
    case YACS::HMI::UPDATEPROGRESS:
      setExecState(type);
      model->setData(modelIndex(YState), 0);
      break;
    default:
      DEBTRACE("SchemaNodeItem::update(), event not handled: " << event);
      SchemaItem::update(event, type, son);
    }
}

void SchemaNodeItem::popupMenu(QWidget *caller, const QPoint &globalPos)
{
  ElementaryNodeMenu m;
  m.popupMenu(caller, globalPos);
}

Qt::ItemFlags SchemaNodeItem::flags(const QModelIndex &index)
{
  return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsUserCheckable | Qt::ItemIsDropEnabled;
}

/*!
 *  drag for nodes in tree are used for control link with Left Mouse Button
 *  and for reparent with Middle Mouse Button
 */
QString SchemaNodeItem::getMimeFormat()
{
  if (QApplication::mouseButtons() == Qt::MidButton)
    return "yacs/subjectNode";
  else
    return "yacs/subjectOutGate";
}

void SchemaNodeItem::toggleState()
{
  DEBTRACE("SchemaNodeItem::toggleState");
  SchemaItem::toggleState();
  GuiExecutor *guiExec = QtGuiContext::getQtCurrent()->getGuiExecutor();
  assert(guiExec);
  SubjectNode *subjectNode = dynamic_cast<SubjectNode*>(getSubject());
  assert(subjectNode);
  string nodeName = QtGuiContext::getQtCurrent()->getProc()->getChildName(subjectNode->getNode());
  DEBTRACE("nodeName=" << nodeName);

  if (_itemCheckState.value(YLabel) == Qt::Checked) // already toggled
    guiExec->addBreakpoint(nodeName);
  else
    guiExec->removeBreakpoint(nodeName);
}

/*!
 *  drop in nodes are used for control link
 */
bool SchemaNodeItem::dropMimeData(const QMimeData* data, Qt::DropAction action)
{
  DEBTRACE("SchemaNodeItem::dropMimeData");
  if (!data) return false;
  const ItemMimeData* myData = dynamic_cast<const ItemMimeData*>(data);
  if (!myData) return false;
  if(!myData->hasFormat("yacs/subjectOutGate")) return false;

  Subject *subFrom = myData->getSubject();
  if (!subFrom) return false;
  SubjectNode* from = dynamic_cast<SubjectNode*>(subFrom);

  SubjectNode *to = dynamic_cast<SubjectNode*>(getSubject());
  if (!to) return false;

  bool ret =false;
  if (from && to)
    {
       ret =true;
       SubjectNode::tryCreateLink(from, to);
    }
  return ret;
}

