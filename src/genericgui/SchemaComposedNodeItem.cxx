
#include <Python.h>

#include "SchemaComposedNodeItem.hxx"
#include "SchemaNodeItem.hxx"
#include "SchemaInPortItem.hxx"
#include "SchemaOutPortItem.hxx"
#include "SchemaComponentItem.hxx"
#include "SchemaContainerItem.hxx"
#include "SchemaDataTypeItem.hxx"
#include "SchemaLinkItem.hxx"
#include "SchemaDirTypesItem.hxx"
#include "SchemaDirContainersItem.hxx"
#include "SchemaDirLinksItem.hxx"
#include "Menus.hxx"
#include "QtGuiContext.hxx"
#include "ItemMimeData.hxx"
#include "GuiEditor.hxx"
#include "RuntimeSALOME.hxx"
#include "Catalog.hxx"

#include "commandsProc.hxx"
#include "ComposedNode.hxx"
#include "guiObservers.hxx"

#include <QIcon>
#include <QMimeData>

#include <cassert>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;
using namespace YACS::ENGINE;
using namespace YACS::HMI;

SchemaComposedNodeItem::SchemaComposedNodeItem(SchemaItem *parent, QString label, Subject* subject)
  : SchemaItem(parent, label, subject)
{
  DEBTRACE("SchemaComposedNodeItem::SchemaComposedNodeItem");
  _itemDeco.replace(YLabel, QIcon("icons:block_node.png"));
  _dirTypesItem = 0;
  _dirContainersItem = 0;
  _dirLinksItem = 0;
  Subject *son = 0;
  int nbsons = childCount();
  SchemaModel *model = QtGuiContext::getQtCurrent()->getSchemaModel();

  model->beginInsertRows(modelIndex(), nbsons, nbsons);
  if (SubjectProc* sProc = dynamic_cast<SubjectProc*>(subject))
    {
      son = new Subject(getSubject());
      _dirTypesItem = new SchemaDirTypesItem(this, "Types", son);
      son = new Subject(getSubject());
      _dirContainersItem = new SchemaDirContainersItem(this, "Containers", son);
    }
  son = new Subject(getSubject());
  _dirLinksItem = new SchemaDirLinksItem(this, "Links", son);
  model->endInsertRows();

  if (SubjectProc* sProc = dynamic_cast<SubjectProc*>(subject))
    {
      Catalog* builtinCatalog = getSALOMERuntime()->getBuiltinCatalog();
      sProc->addDataType(builtinCatalog, "bool");
      sProc->addDataType(builtinCatalog, "int");
      sProc->addDataType(builtinCatalog, "double");
      sProc->addDataType(builtinCatalog, "string");
    }
  if (!model->isEdition())
    {
      setExecState(YACS::UNDEFINED);
    }
}

SchemaComposedNodeItem::~SchemaComposedNodeItem()
{
  DEBTRACE("SchemaComposedNodeItem::~SchemaComposedNodeItem");
}

void SchemaComposedNodeItem::update(GuiEvent event, int type, Subject* son)
{
  DEBTRACE("SchemaComposedNodeItem::update "<<event<<" "<<type<<" "<<son);
  SchemaModel *model = QtGuiContext::getQtCurrent()->getSchemaModel();
  SchemaItem *item = 0;
  SubjectNode *snode = 0;
  Node* node = 0;
  switch (event)
    {
    case YACS::HMI::ADD:
      switch (type)
        {
        case YACS::HMI::BLOC:
        case YACS::HMI::FORLOOP:
        case YACS::HMI::WHILELOOP:
        case YACS::HMI::SWITCH:
        case YACS::HMI::FOREACHLOOP:
        case YACS::HMI::OPTIMIZERLOOP:
          {
            int nbsons = childCount();
            model->beginInsertRows(modelIndex(), nbsons, nbsons);
            item =  new SchemaComposedNodeItem(this,
                                               son->getName().c_str(),
                                               son);
            model->endInsertRows();
          }
          break;
        case YACS::HMI::PYTHONNODE:
        case YACS::HMI::PYFUNCNODE:
        case YACS::HMI::CORBANODE:
        case YACS::HMI::CPPNODE:
        case YACS::HMI::SALOMENODE:
        case YACS::HMI::SALOMEPYTHONNODE:
        case YACS::HMI::XMLNODE:
        case YACS::HMI::PRESETNODE:
        case YACS::HMI::OUTNODE:
        case YACS::HMI::STUDYINNODE:
        case YACS::HMI::STUDYOUTNODE:
          {
            int nbsons = childCount();
            model->beginInsertRows(modelIndex(), nbsons, nbsons);
            item =  new SchemaNodeItem(this,
                                       son->getName().c_str(),
                                       son);
            model->endInsertRows();
          }
          break;
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
        case YACS::HMI::COMPONENT:
          {
            assert(_dirContainersItem);
            _dirContainersItem->addComponentItem(son);
          }
          break;
        case YACS::HMI::CONTAINER:
          {
            assert(_dirContainersItem);
            _dirContainersItem->addContainerItem(son);
          }
          break;
        case YACS::HMI::DATATYPE:
          {
            assert(_dirTypesItem);
            _dirTypesItem->addTypeItem(son);
          }
          break;
        default:
          DEBTRACE("SchemaComposedNodeItem::update() ADD, type not handled:" << type);
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
          model->setData(modelIndex(), 0);
          break;
        case YACS::READY:
          _itemForeground.replace(YLabel, QColor("blue"));
          model->setData(modelIndex(), 0);
              break;
        default:
          break;
        }
      break;
    case YACS::HMI::ADDLINK:
    case YACS::HMI::ADDCONTROLLINK:
          {
            assert(_dirLinksItem);
            _dirLinksItem->addLinkItem(son);
          }
      break;
    case YACS::HMI::UPDATEPROGRESS:
      setExecState(type);
      model->setData(modelIndex(YState), 0);
      break;
    default:
      DEBTRACE("SchemaComposedNodeItem::update(), event not handled: "<< event);
      SchemaItem::update(event, type, son);
    }
}

std::list<YACS::ENGINE::Node*> SchemaComposedNodeItem::getDirectDescendants() const
{
  SubjectNode* SNode = dynamic_cast<SubjectNode*>(_subject);
  assert(SNode);
  ComposedNode* CNode = dynamic_cast<ComposedNode*>(SNode->getNode());
  assert(CNode);
  return CNode->edGetDirectDescendants();
}

void SchemaComposedNodeItem::popupMenu(QWidget *caller, const QPoint &globalPos)
{
  ComposedNodeMenu m;
  m.popupMenu(caller, globalPos);
}

Qt::ItemFlags SchemaComposedNodeItem::flags(const QModelIndex &index)
{
  //DEBTRACE("SchemaComposedNodeItem::flags");
  return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
}

/*!
 *  drop in composed nodes are used for add nodes from catalog, or reparent nodes,
 *  or control link
 */
bool SchemaComposedNodeItem::dropMimeData(const QMimeData* data, Qt::DropAction action)
{
  DEBTRACE("SchemaComposedNodeItem::dropMimeData");
  if (!data) return false;
  const ItemMimeData* myData = dynamic_cast<const ItemMimeData*>(data);
  if (!myData) return false;
  bool ret =false;
  if (myData->hasFormat("yacs/cataService") || myData->hasFormat("yacs/cataNode"))
    {
      ret =true;
      SubjectComposedNode *cnode = dynamic_cast<SubjectComposedNode*>(getSubject());
      QtGuiContext::getQtCurrent()->getGMain()->_guiEditor->CreateNodeFromCatalog(myData, cnode);
    }
  else if(myData->hasFormat("yacs/subjectNode"))
    {
      ret = true;
      Subject *sub = myData->getSubject();
      if (!sub) return false;
      SubjectNode *node = dynamic_cast<SubjectNode*>(sub);
      if (!node) return false;
      SubjectComposedNode *cnode = dynamic_cast<SubjectComposedNode*>(getSubject());
      if (cnode) node->reparent(cnode);
    }
  else if(myData->hasFormat("yacs/subjectOutGate"))
    {
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
    }
  return ret;
}

/*!
 *  drag for  composed nodes in tree are used for control link with Left Mouse Button
 *  and for reparent with Middle Mouse Button
 */
QString SchemaComposedNodeItem::getMimeFormat()
{
  if (QApplication::mouseButtons() == Qt::MidButton)
    return "yacs/subjectNode";
  else
    return "yacs/subjectOutGate";
}
