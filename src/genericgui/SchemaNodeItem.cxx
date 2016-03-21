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

#include "SchemaNodeItem.hxx"
#include "SchemaInPortItem.hxx"
#include "SchemaOutPortItem.hxx"
#include "InputPort.hxx"
#include "OutputPort.hxx"
#include "ItemMimeData.hxx"
#include "QtGuiContext.hxx"
#include "GuiExecutor.hxx"
#include "Menus.hxx"
#include "Message.hxx"

#include "Node.hxx"
#include "Switch.hxx"

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
  setCaseValue();
}

SchemaNodeItem::~SchemaNodeItem()
{
  DEBTRACE("SchemaNodeItem::~SchemaNodeItem");
}

void SchemaNodeItem::update(GuiEvent event, int type, Subject* son)
{
  DEBTRACE("SchemaNodeItem::update "<<eventName(event)<<" "<<type<<" "<<son);
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
//         default:
//           DEBTRACE("SchemaNodeItem::update(), ADD, type not handled: " << type);
        }
      break;
    case YACS::HMI::ORDER:
      {
        YASSERT(QtGuiContext::getQtCurrent()->_mapOfSchemaItem.count(son));
        //bool isInput = dynamic_cast<SubjectInputPort*>(son);

        snode = dynamic_cast<SubjectNode*>(_subject);
        YASSERT(snode);
        Node* node = snode->getNode();
        ElementaryNode* father = dynamic_cast<ElementaryNode*>(node);
        YASSERT(father);
        int nbChildren = childCount();

        model->beginRemoveRows(modelIndex(), 0, nbChildren-1);
        for (int i = nbChildren; i >= 0; i--)
          removeChild(child(i));
        model->endRemoveRows();

        list<InputPort*> plisti = father->getSetOfInputPort();
        int nbIn = plisti.size();
        if (nbIn)
          {
            model->beginInsertRows(modelIndex(), 0, nbIn-1);
            list<InputPort*>::iterator iti = plisti.begin();
            for(; iti != plisti.end(); iti++)
              {
                Subject *sub = QtGuiContext::getQtCurrent()->_mapOfSubjectDataPort[(*iti)];
                SchemaItem *item = QtGuiContext::getQtCurrent()->_mapOfSchemaItem[sub];
                appendChild(item);
              }
            model->endInsertRows();
          }

        list<OutputPort*> plisto = father->getSetOfOutputPort();
        int nbOut = plisto.size();
        if (nbOut)
          {
            model->beginInsertRows(modelIndex(), nbIn, nbIn + nbOut -1);
            list<OutputPort*>::iterator ito = plisto.begin();
            for(; ito != plisto.end(); ito++)
              {
                Subject *sub = QtGuiContext::getQtCurrent()->_mapOfSubjectDataPort[(*ito)];
                SchemaItem *item = QtGuiContext::getQtCurrent()->_mapOfSchemaItem[sub];
                appendChild(item);
              }
            model->endInsertRows();
          }
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
          model->setData(modelIndex(YLabel), 0);  // --- to emit dataChanged signal
          break;
        case YACS::READY:
          _itemForeground.replace(YLabel, QColor("blue"));
          model->setData(modelIndex(YLabel), 0);
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
      //DEBTRACE("SchemaNodeItem::update(), event not handled: " << eventName(event));
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
  Qt::ItemFlags pflag = Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsDropEnabled;
  if ( !QtGuiContext::getQtCurrent() || !QtGuiContext::getQtCurrent()->isEdition())
    return pflag;

  if (QtGuiContext::getQtCurrent()->isEdition())
    pflag = pflag | Qt::ItemIsDragEnabled;
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
  YASSERT(guiExec);
  SubjectNode *subjectNode = dynamic_cast<SubjectNode*>(getSubject());
  YASSERT(subjectNode);
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
       if (!SubjectNode::tryCreateLink(from, to))
         Message mess;
    }
  return ret;
}

void SchemaNodeItem::setCaseValue()
{
  DEBTRACE("SchemaNodeItem::setCaseValue");
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

QVariant SchemaNodeItem::editionWhatsThis(int column) const
{
  return "<p>To edit the node properties, select the node and use the input panel. <a href=\"modification.html#property-page-for-node\">More...</a></p>";
}
