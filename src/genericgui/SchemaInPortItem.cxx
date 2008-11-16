#include "SchemaInPortItem.hxx"
#include "ItemMimeData.hxx"
#include "QtGuiContext.hxx"
#include "Menus.hxx"

#include "commandsProc.hxx"
#include "DataPort.hxx"
#include "TypeCode.hxx"
#include "InputPort.hxx"
#include "OutputPort.hxx"

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
  : SchemaItem::SchemaItem(parent, label, subject)
{
  SubjectDataPort *subPort = dynamic_cast<SubjectDataPort*>(subject);
  if (subPort)
    {
      DataPort *dport = subPort->getPort();
      TypeOfElem typort = ProcInvoc::getTypeOfPort(dport);
//       _itemData.replace(YType, dport->edGetType()->getKindRepr());
      _itemData.replace(YType, dport->edGetType()->name());
      _itemForeground.replace(YType, QColor("black"));
      InputPort *inport = 0;

      switch (typort)
        {
        case YACS::HMI::INPUTPORT:
          _itemDeco.replace(YLabel, QIcon("icons:in_port.png"));
          inport = dynamic_cast<InputPort*>(dport);
          if (inport->edIsManuallyInitialized())
            {
              _itemData.replace(YValue, inport->valToStr().c_str());
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
            DataFlowPort *port = dynamic_cast<DataFlowPort*>(sip->getPort());
            DEBTRACE(port->valToStr());
            _itemData.replace(YValue, port->valToStr().c_str());
            _itemForeground.replace(YValue, QColor("green"));
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
      flagEdit = Qt::ItemIsEditable; // --- port name editable
      break;
    case 2:
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
          SubjectDataPort::tryCreateLink(from, to);
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

