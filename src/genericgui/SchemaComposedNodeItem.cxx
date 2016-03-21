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

#include <Python.h>

#include "Resource.hxx"
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
#include "Message.hxx"
#include "QtGuiContext.hxx"
#include "ItemMimeData.hxx"
#include "GuiEditor.hxx"
#include "RuntimeSALOME.hxx"
#include "Catalog.hxx"

#include "commandsProc.hxx"
#include "ComposedNode.hxx"
#include "guiObservers.hxx"
#include "Node.hxx"
#include "Switch.hxx"

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
  switch (subject->getType())
    {
    case YACS::HMI::BLOC: 
      _itemDeco.replace(YLabel, QIcon("icons:block_node.png"));
      break;
    case YACS::HMI::FORLOOP: 
    case YACS::HMI::FOREACHLOOP: 
    case YACS::HMI::WHILELOOP: 
    case YACS::HMI::OPTIMIZERLOOP: 
      _itemDeco.replace(YLabel, QIcon("icons:loop_node.png"));
      break;
    case YACS::HMI::SWITCH: 
      _itemDeco.replace(YLabel, QIcon("icons:switch_node.png"));
      break;
    default:
      _itemDeco.replace(YLabel, QIcon("icons:block_node.png"));
    }
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

  if (!model->isEdition())
    {
      setExecState(YACS::UNDEFINED);
    }

  SubjectComposedNode *scn = dynamic_cast<SubjectComposedNode*>(subject);
  YASSERT(scn);
  if (scn->hasValue())
    {
      _itemData.replace(YType, scn->getValue().c_str());
      model->setData(modelIndex(YType), 0);
    }
 
  setCaseValue();
}

SchemaComposedNodeItem::~SchemaComposedNodeItem()
{
  DEBTRACE("SchemaComposedNodeItem::~SchemaComposedNodeItem");
}

void SchemaComposedNodeItem::update(GuiEvent event, int type, Subject* son)
{
  DEBTRACE("SchemaComposedNodeItem::update "<<eventName(event)<<" "<<type<<" "<<son);
  SchemaModel *model = QtGuiContext::getQtCurrent()->getSchemaModel();
  SchemaItem *item = 0;
  SubjectNode *snode = 0;
  SubjectComposedNode *scnode = 0;
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
            YASSERT(_dirContainersItem);
            _dirContainersItem->addComponentItem(son);
          }
          break;
        case YACS::HMI::CONTAINER:
          {
            YASSERT(_dirContainersItem);
            _dirContainersItem->addContainerItem(son);
          }
          break;
        case YACS::HMI::DATATYPE:
          {
            YASSERT(_dirTypesItem);
            _dirTypesItem->addTypeItem(son);
          }
          break;
        }
      break;

    case YACS::HMI::REMOVE:
      switch (type)
        {
        case YACS::HMI::DATATYPE:
          {
            YASSERT(_dirTypesItem);
            _dirTypesItem->removeTypeItem(son);
          }
          break;
        }
      break;

    case YACS::HMI::UPDATE:
      snode = dynamic_cast<SubjectNode*>(_subject);
      YASSERT(snode);
      node = snode->getNode();
      YASSERT(node);
      switch (node->getState())
        {
        case YACS::INVALID:
          _itemForeground.replace(YLabel, QColor("red"));
          model->setData(modelIndex(YLabel), 0);
          break;
        case YACS::READY:
          _itemForeground.replace(YLabel, QColor("blue"));
          model->setData(modelIndex(YLabel), 0);
              break;
        default:
          break;
        }
      break;
    case YACS::HMI::ADDLINK:
    case YACS::HMI::ADDCONTROLLINK:
          {
            YASSERT(_dirLinksItem);
            _dirLinksItem->addLinkItem(son);
          }
      break;
    case YACS::HMI::UPDATEPROGRESS:
      setExecState(type);
      model->setData(modelIndex(YState), 0);
      break;
    case YACS::HMI::SETVALUE:
      scnode = dynamic_cast<SubjectComposedNode*>(_subject);
      if (scnode->hasValue())
        {
          _itemData.replace(YType, scnode->getValue().c_str());
          model->setData(modelIndex(YType), 0);
        }
    case YACS::HMI::SETCASE:
      {
        SubjectSwitch *sSwitch = dynamic_cast<SubjectSwitch*>(_subject);
        if (sSwitch)
          {
            Switch *aSwitch = dynamic_cast<Switch*>(sSwitch->getNode());
            Node *node = aSwitch->edGetNode(type);
            if (node)
              {
                if (GuiContext::getCurrent()->_mapOfSubjectNode.count(node))
                  {
                    Subject* sub = GuiContext::getCurrent()->_mapOfSubjectNode[node];
                    if (QtGuiContext::getQtCurrent()->_mapOfSchemaItem.count(sub))
                      {
                        SchemaItem* item = QtGuiContext::getQtCurrent()->_mapOfSchemaItem[sub];
                        item->setCaseValue();
                      }
                  }
              }
          }
      }
      break;
    case YACS::HMI::SETSELECT:
      {
        SubjectSwitch *sSwitch = dynamic_cast<SubjectSwitch*>(_subject);
        if (sSwitch && sSwitch->hasValue())
          {
            _itemData.replace(YType, sSwitch->getValue().c_str());
            model->setData(modelIndex(YType), 0);
          }
      }
      break;
    case YACS::HMI::CUT:
      {
        SchemaItem *toMove = QtGuiContext::getQtCurrent()->_mapOfSchemaItem[son];
        int position = toMove->row();
        model->beginRemoveRows(modelIndex(), position, position);
        removeChild(toMove);
        model->endRemoveRows();
      }
      break;
    case YACS::HMI::PASTE:
      {
        SchemaItem *toPaste = QtGuiContext::getQtCurrent()->_mapOfSchemaItem[son];
        int nbsons = childCount();
        model->beginInsertRows(modelIndex(), nbsons, nbsons);
        toPaste->reparent(this);
        model->endInsertRows();
      }
      break;
    default:
      //DEBTRACE("SchemaComposedNodeItem::update(), event not handled: "<< eventName(event));
      SchemaItem::update(event, type, son);
    }
}

std::list<YACS::ENGINE::Node*> SchemaComposedNodeItem::getDirectDescendants() const
{
  SubjectNode* SNode = dynamic_cast<SubjectNode*>(_subject);
  YASSERT(SNode);
  ComposedNode* CNode = dynamic_cast<ComposedNode*>(SNode->getNode());
  YASSERT(CNode);
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
  Qt::ItemFlags pflag = Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
  if (!QtGuiContext::getQtCurrent() || !QtGuiContext::getQtCurrent()->isEdition())
    return pflag;

  Qt::ItemFlags flagEdit = 0;
  int column = index.column();
  switch (column)
    {
    case YValue:
      flagEdit = Qt::ItemIsEditable; // --- item value editable in model view (for node case in switch)
      break;     
    }

  return pflag | flagEdit;

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
      bool createNewComponentInstance=Resource::COMPONENT_INSTANCE_NEW;
      // by default getControl gives false. In this case we use the user preference COMPONENT_INSTANCE_NEW
      // to create the node. If getControl gives true we invert the user preference
      if(myData->getControl())
        createNewComponentInstance=!Resource::COMPONENT_INSTANCE_NEW;
      QtGuiContext::getQtCurrent()->getGMain()->_guiEditor->CreateNodeFromCatalog(myData, cnode,createNewComponentInstance);
    }
  else if(myData->hasFormat("yacs/subjectNode"))
    {
      ret = true;
      Subject *sub = myData->getSubject();
      if (!sub) return false;
      SubjectNode *node = dynamic_cast<SubjectNode*>(sub);
      if (!node) return false;
      SubjectComposedNode *cnode = dynamic_cast<SubjectComposedNode*>(getSubject());
      if (cnode)
        if (! node->reparent(cnode))
          Message mess;
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
          if (!SubjectNode::tryCreateLink(from, to))
            Message mess;
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

void SchemaComposedNodeItem::setCaseValue()
{
  DEBTRACE("SchemaComposedNodeItem::setCaseValue");
  Subject *sub = _parentItem->getSubject();
  SubjectSwitch *sSwitch = dynamic_cast<SubjectSwitch*>(sub);
  if (!sSwitch) return;

  SchemaModel *model = QtGuiContext::getQtCurrent()->getSchemaModel();
  Switch *aSwitch = dynamic_cast<Switch*>(sSwitch->getNode());
  YASSERT(aSwitch);
  SubjectNode *sNode = dynamic_cast<SubjectNode*>(_subject);
  YASSERT(sNode);
  int rank = aSwitch->getRankOfNode(sNode->getNode());
  if (rank == Switch::ID_FOR_DEFAULT_NODE)
    _itemData.replace(YValue, "default");
  else
    _itemData.replace(YValue, rank);
  model->setData(modelIndex(YValue), 0);
}

QVariant SchemaComposedNodeItem::editionWhatsThis(int column) const
{
  return "<p>To edit the node properties, select the node and use the input panel. <a href=\"modification.html#property-page-for-block-node\">More...</a></p>";
}
