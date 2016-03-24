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

#include "SchemaInPortItem.hxx"
#include "ItemMimeData.hxx"
#include "QtGuiContext.hxx"
#include "Menus.hxx"
#include "Message.hxx"

#include "commandsProc.hxx"
#include "DataPort.hxx"
#include "TypeCode.hxx"
#include "InputPort.hxx"
#include "OutputPort.hxx"
#include "DataNode.hxx"
#include "InlineNode.hxx"

#include <QIcon>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;
using namespace YACS::ENGINE;
using namespace YACS::HMI;

/*! column 1 = name, set in SchemaItem,
 *  column 2 = data type, never editable,
 *  column 3 = value
 */
SchemaInPortItem::SchemaInPortItem(SchemaItem *parent, QString label, Subject* subject)
  : SchemaItem(parent, label, subject)
{
  SubjectDataPort *subPort = dynamic_cast<SubjectDataPort*>(subject);
  _isDataStream = false;
  if (subPort)
    {
      DataPort *dport = subPort->getPort();
      TypeOfElem typort = ProcInvoc::getTypeOfPort(dport);
      TypeOfElem typnode = ProcInvoc::getTypeOfNode(dport->getNode());
//       _itemData.replace(YType, dport->edGetType()->getKindRepr());
      _itemData.replace(YType, dport->edGetType()->name());
      _itemForeground.replace(YType, QColor("black"));
      InputPort *inport = 0;

      if(typnode==YACS::HMI::STUDYOUTNODE)
        {
          //It's a study out node
          if(typort==YACS::HMI::INPUTPORT)
            {
              _itemDeco.replace(YLabel, QIcon("icons:in_port.png"));
              inport = dynamic_cast<InputPort*>(dport);
              std::string val=inport->getAsString();
              if(val != "")
                {
                  _itemData.replace(YValue, val.c_str());
                  if (inport->edGetNumberOfLinks())
                    _itemForeground.replace(YValue, QColor("green"));
                  else
                    _itemForeground.replace(YValue, QColor("red"));
                }
              else
                {
                  _itemData.replace(YValue, "not initialized");
                  _itemForeground.replace(YValue, QColor("red"));
                }
            }
          return;
        }

      switch (typort)
        {
        case YACS::HMI::INPUTPORT:
          _isDataStream = false;
          _itemDeco.replace(YLabel, QIcon("icons:in_port.png"));
          inport = dynamic_cast<InputPort*>(dport);
          if (inport->edIsManuallyInitialized())
            {
              _itemData.replace(YValue, inport->getAsString().c_str());
              _itemForeground.replace(YValue, QColor("green"));
            }
          else if (inport->edGetNumberOfLinks())
            {
              _itemData.replace(YValue, "linked");
              _itemForeground.replace(YValue, QColor("blue"));
            }
          else
            {
              _itemData.replace(YValue, "not initialized");
              _itemForeground.replace(YValue, QColor("red"));
            }
          break;
        case YACS::HMI::INPUTDATASTREAMPORT:
          _isDataStream = true;
          _itemDeco.replace(YLabel, QIcon("icons:in_port.png"));
          _itemData.replace(YValue, "stream");
          _itemForeground.replace(YValue, QColor("grey"));
          break;
        }
    }
}

void SchemaInPortItem::update(GuiEvent event, int type, Subject* son)
{
  DEBTRACE("SchemaInPortItem::update");
  SchemaItem::update(event, type, son);
  QModelIndex index = QModelIndex();
  SchemaModel *model = QtGuiContext::getQtCurrent()->getSchemaModel();
  switch (event)
    {
    case SETVALUE:
      {
        SubjectInputPort *sip = dynamic_cast<SubjectInputPort*>(son);
        if (sip)
          {
            InputPort* port = dynamic_cast<InputPort*>(sip->getPort());
            TypeOfElem typnode = ProcInvoc::getTypeOfNode(port->getNode());
            DEBTRACE(port->getAsString());
            if(typnode==YACS::HMI::STUDYOUTNODE)
              {
                if(port->getAsString().empty())
                  {
                    _itemData.replace(YValue, "not initialized");
                    if (port->edGetNumberOfLinks())
                      _itemForeground.replace(YValue, QColor("blue"));
                    else
                      _itemForeground.replace(YValue, QColor("red"));
                  }
                else
                  {
                    _itemData.replace(YValue, port->getAsString().c_str());
                    if (port->edGetNumberOfLinks())
                      _itemForeground.replace(YValue, QColor("green"));
                    else
                      _itemForeground.replace(YValue, QColor("red"));
                  }
              }
            else
              {
                _itemData.replace(YValue, port->getAsString().c_str());
                _itemForeground.replace(YValue, QColor("green"));
              }
            model->setData(modelIndex(YValue), 0); // --- to emit dataChanged signal
          }
      }
      break;
    case UPDATE:
      {
        SubjectInputPort *sip = dynamic_cast<SubjectInputPort*>(_subject);
        if (sip)
          {
            InputPort* port = dynamic_cast<InputPort*>(sip->getPort());
            TypeOfElem typnode = ProcInvoc::getTypeOfNode(port->getNode());
            TypeOfElem typort = ProcInvoc::getTypeOfPort(port);
            _itemData.replace(YType, port->edGetType()->name());
            _itemForeground.replace(YType, QColor("black"));
            if(typnode==YACS::HMI::STUDYOUTNODE)
              {
                if(port->getAsString().empty())
                  {
                    _itemData.replace(YValue, "not initialized");
                    if (port->edGetNumberOfLinks())
                      _itemForeground.replace(YValue, QColor("blue"));
                    else
                      _itemForeground.replace(YValue, QColor("red"));
                  }
                else
                  {
                    _itemData.replace(YValue, port->getAsString().c_str());
                    if (port->edGetNumberOfLinks())
                      _itemForeground.replace(YValue, QColor("green"));
                    else
                      _itemForeground.replace(YValue, QColor("red"));
                  }
              }
            else
              {
                if (port->edGetNumberOfLinks())
                  {
                    _itemData.replace(YValue, "linked");
                    _itemForeground.replace(YValue, QColor("blue"));
                  }
                else if (port->edIsManuallyInitialized())
                  {
                    _itemData.replace(YValue, port->getAsString().c_str());
                    _itemForeground.replace(YValue, QColor("green"));
                  }
                else
                  {
                    _itemData.replace(YValue, "not initialized");
                    _itemForeground.replace(YValue, QColor("red"));
                  }
              }
            model->setData(modelIndex(YValue), 0); // --- to emit dataChanged signal
          }
      }
      break;
    case UPDATEPROGRESS:
      {
        SubjectInputPort *sip = dynamic_cast<SubjectInputPort*>(son);
        if (sip)
          {
            _itemData.replace(YValue, sip->getExecValue().c_str());
            _itemForeground.replace(YValue, QColor("darkCyan"));
            model->setData(modelIndex(YValue), 0); // --- to emit dataChanged signal
          }
      }
      break;      
    }
}

Qt::ItemFlags SchemaInPortItem::flags(const QModelIndex &index)
{
  //DEBTRACE("SchemaInPortItem::flags");
  Qt::ItemFlags pflag = Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDropEnabled;

  Qt::ItemFlags flagEdit = 0;
  int column = index.column();
  switch (column)
    {
    case 0:
      {
        SubjectDataPort *sdp = dynamic_cast<SubjectDataPort*>(_subject);
        Node *parent = sdp->getPort()->getNode();
        if (parent)
          if ( (dynamic_cast<DataNode*>(parent) || dynamic_cast<InlineNode*>(parent)) &&
               QtGuiContext::getQtCurrent()->isEdition() )
            flagEdit = Qt::ItemIsEditable; // --- port name editable
      }
      break;
    case 2:
      if (!_isDataStream)
        flagEdit = Qt::ItemIsEditable; // --- port value editable      
    }
  return pflag | flagEdit;
}

bool SchemaInPortItem::dropMimeData(const QMimeData* data, Qt::DropAction action)
{
  DEBTRACE("SchemaInPortItem::dropMimeData");
  if (!data) return false;
  const ItemMimeData* myData = dynamic_cast<const ItemMimeData*>(data);
  if (!myData) return false;
  if(!myData->hasFormat("yacs/subjectOutPort")) return false;
  SubjectDataPort *to = dynamic_cast<SubjectDataPort*>(getSubject());
  if (!to) return false;
  InPort *toport = dynamic_cast<InPort*>(to->getPort());
  if (!toport) return false;

  bool ret =false;
  TypeOfElem typort = ProcInvoc::getTypeOfPort(toport);
  switch (typort)
    {
    case YACS::HMI::INPUTPORT:
    case YACS::HMI::INPUTDATASTREAMPORT:
      {
        ret =true;
        Subject *sub = myData->getSubject();
        if (!sub) break;
        SubjectDataPort* from = dynamic_cast<SubjectDataPort*>(sub);
        if (from && to)
          if (!SubjectDataPort::tryCreateLink(from, to,myData->getControl()))
            Message mess;
        break;
      }
    default:
      break;
    }
  return ret;
}

void SchemaInPortItem::popupMenu(QWidget *caller, const QPoint &globalPos)
{
  InPortMenu m;
  m.popupMenu(caller, globalPos);
}

QVariant SchemaInPortItem::editionWhatsThis(int column) const
{
  QString answer;
  if (_isDataStream)
    answer = "Edition, DataStream input port ";
  else
    answer = "Edition, DataFlow input port ";
  switch (column)
    {
    case YLabel:
      answer += "name=";
      answer += _itemData.value(column).toString();
      answer += "\nInput port is edited in the input panel of the node.\n" \
        "Select the node to get access to it's input panel.";
      break;
    case YType:
      answer += "type=";
      answer += _itemData.value(column).toString();
      answer += "\nInput port is edited in the input panel of the node.\n" \
        "Select the node to get access to it's input panel.\n" \
        "Type is not modifiable, you must delete and recreate the port. Existing links will be lost.";
      break;
    case YValue:
      answer += "value=";
      answer += _itemData.value(column).toString();
      answer += "\nInput port is edited in the input panel of the node.\n" \
        "Select the node to get access to it's input panel.\n";
      if (_isDataStream)
        answer += "DataStream ports have no editable value, they must be linked to an output DataStream port.";
      else
        {
          answer += "DataFlow ports must be either manually initialized or linked to an output DataFlow port. "; 
          answer += "If the port is manually initialized and linked, the value from the link will prevail ";
          answer += "if it is available when the node is executed.";
        }
      break;
    }
  return answer;
}


