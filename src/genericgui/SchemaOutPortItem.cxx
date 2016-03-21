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

#include "SchemaOutPortItem.hxx"
#include "ItemMimeData.hxx"
#include "QtGuiContext.hxx"
#include "Menus.hxx"

#include "commandsProc.hxx"
#include "DataPort.hxx"
#include "TypeCode.hxx"
#include "InputPort.hxx"
#include "OutputPort.hxx"
#include "PresetNode.hxx"
#include "DataNode.hxx"
#include "InlineNode.hxx"

#include <QIcon>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;
using namespace YACS::ENGINE;
using namespace YACS::HMI;

/*! column 1: YLabel = name, set in SchemaItem,
 *  column 2: YType  = data type, never editable,
 *  column 3: YValue = value
 */
SchemaOutPortItem::SchemaOutPortItem(SchemaItem *parent, QString label, Subject* subject)
  : SchemaItem(parent, label, subject)
{
  SubjectDataPort *subPort = dynamic_cast<SubjectDataPort*>(subject);
  if (subPort)
    {
      DataPort *dport = subPort->getPort();
      TypeOfElem typort = ProcInvoc::getTypeOfPort(dport);
//       _itemData.replace(YType, dport->edGetType()->getKindRepr());
      _itemData.replace(YType, dport->edGetType()->name());
      _itemForeground.replace(YType, QColor("black"));
      OutputPort * outport = 0;

      switch (typort)
        {
        case YACS::HMI::OUTPUTPORT:
          _itemDeco.replace(YLabel, QIcon("icons:out_port.png"));
          outport = dynamic_cast<OutputPort*>(dport);
          _itemData.replace(YValue, outport->getAsString().c_str());
          break;
        case YACS::HMI::OUTPUTDATASTREAMPORT:
          _itemDeco.replace(YLabel, QIcon("icons:out_port.png"));
          _itemData.replace(YValue, "stream");
          _itemForeground.replace(YValue, QColor("grey"));
          //_itemDeco.replace(YValue, QColor("grey"));
          break;
        }
    }
}

void SchemaOutPortItem::update(GuiEvent event, int type, Subject* son)
{
  DEBTRACE("SchemaOutPortItem::update");
  SchemaItem::update(event, type, son);
  QModelIndex index = QModelIndex();
  SchemaModel *model = QtGuiContext::getQtCurrent()->getSchemaModel();
  switch (event)
    {
    case SETVALUE:
      {
        SubjectOutputPort *sop = dynamic_cast<SubjectOutputPort*>(son);
        if (sop)
          {
            DataFlowPort *port = dynamic_cast<DataFlowPort*>(sop->getPort());
            DEBTRACE(port->getAsString());
            _itemData.replace(YValue, port->getAsString().c_str());
            _itemForeground.replace(YValue, QColor("green"));
            model->setData(modelIndex(YValue), 0); // --- to emit dataChanged signal
          }
      }
      break;
    case UPDATE:
      {
        SubjectOutputPort *sop = dynamic_cast<SubjectOutputPort*>(son);
        if (sop)
          {
            DataFlowPort *port = dynamic_cast<DataFlowPort*>(sop->getPort());
            _itemData.replace(YType, port->edGetType()->name());
            _itemForeground.replace(YType, QColor("black"));
          }
      }
      break;
    case UPDATEPROGRESS:
      {
        SubjectOutputPort *sip = dynamic_cast<SubjectOutputPort*>(son);
        if (sip)
          {
            _itemData.replace(YValue, sip->getExecValue().c_str());
            _itemForeground.replace(YValue, QColor("darkMagenta"));
            model->setData(modelIndex(YValue), 0); // --- to emit dataChanged signal
          }
      }
      break;      
    }
}

Qt::ItemFlags SchemaOutPortItem::flags(const QModelIndex &index)
{
  //DEBTRACE("SchemaOutPortItem::flags");
  Qt::ItemFlags pflag = Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDropEnabled;
  if (QtGuiContext::getQtCurrent()->isEdition())
    pflag = pflag | Qt::ItemIsDragEnabled;
  else
    return pflag;

  Qt::ItemFlags flagEdit = 0;
  int column = index.column();
  switch (column)
    {
    case 0:
      {
        SubjectDataPort *sdp = dynamic_cast<SubjectDataPort*>(_subject);
        Node *parent = sdp->getPort()->getNode();
        if (parent)
          if (dynamic_cast<DataNode*>(parent) || dynamic_cast<InlineNode*>(parent))
            flagEdit = Qt::ItemIsEditable; // --- port name editable
      }
      break;
    case 2:
      SubjectDataPort *sdp = dynamic_cast<SubjectDataPort*>(_subject);
      Node *node = sdp->getPort()->getNode();
      //PresetNode *pnode = dynamic_cast<PresetNode*>(node);
      DataNode *pnode = dynamic_cast<DataNode*>(node);
      if (! pnode) break;
      flagEdit = Qt::ItemIsEditable; // --- port value editable for preset node
      break;   
    }
  return pflag | flagEdit;
}

void SchemaOutPortItem::popupMenu(QWidget *caller, const QPoint &globalPos)
{
  OutPortMenu m;
  m.popupMenu(caller, globalPos);
}

QString SchemaOutPortItem::getMimeFormat()
{
  return "yacs/subjectOutPort";
}
