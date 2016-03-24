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

#include "SchemaProcItem.hxx"
#include "Menus.hxx"
#include "QtGuiContext.hxx"
#include "Resource.hxx"

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;
using namespace YACS::ENGINE;
using namespace YACS::HMI;


SchemaProcItem::SchemaProcItem(SchemaItem *parent, QString label, Subject* subject)
  : SchemaComposedNodeItem(parent, label, subject)
{
  DEBTRACE("SchemaProcItem::SchemaProcItem");
}

SchemaProcItem::~SchemaProcItem()
{
  DEBTRACE("SchemaProcItem::~SchemaProcItem");
}

void SchemaProcItem::popupMenu(QWidget *caller, const QPoint &globalPos)
{
  ProcMenu m;
  m.popupMenu(caller, globalPos);
}

void SchemaProcItem::setExecState(int execState)
{
  DEBTRACE("SchemaProcItem::setExecState " << execState);
  _execState = execState;
  QString stateDef;
  QColor sc;
  switch (_execState)
    {
    case YACS::NOTYETINITIALIZED: sc = YACS::HMI::Resource::NOTYETINITIALIZED; stateDef = "Not Yet Initialized"; break;
    case YACS::INITIALISED:       sc = YACS::HMI::Resource::INITIALISED      ; stateDef = "Initialized";         break;
    case YACS::RUNNING:           sc = YACS::HMI::Resource::RUNNING          ; stateDef = "Running";             break;
    case YACS::WAITINGTASKS:      sc = YACS::HMI::Resource::WAITINGTASKS     ; stateDef = "Waiting Tasks";       break;
    case YACS::PAUSED:            sc = YACS::HMI::Resource::PAUSED           ; stateDef = "Paused";              break;
    case YACS::FINISHED:          sc = YACS::HMI::Resource::FINISHED         ; stateDef = "Finished";            break;
    case YACS::STOPPED:           sc = YACS::HMI::Resource::STOPPED          ; stateDef = "Stopped";             break;
    default:                      sc = YACS::HMI::Resource::UNKNOWN          ; stateDef = "Unknown Status";
   }
  _itemData.replace(YState, stateDef);
  _itemForeground.replace(YState, QColor(Qt::darkBlue));
  _itemBackground.replace(YState, sc);
}

QVariant SchemaProcItem::editionToolTip(int column) const
{
  QString val = QString("Edition: ") + _itemData.value(0).toString();
  QString val1 = _itemData.value(2).toString();
  QString val2 = QtGuiContext::getQtCurrent()->getFileName();
  if (!val1.isEmpty()) val += QString(" | ") + val1;
  if (!val2.isEmpty()) val += QString(" | ") + val2;
  return val;
}

QVariant SchemaProcItem::runToolTip(int column) const
{
  QString val = QString("Execution: ") + _itemData.value(0).toString();
  QString val1 = _itemData.value(2).toString();
  QString val2 = QtGuiContext::getQtCurrent()->getFileName();
  if (!val1.isEmpty()) val += QString(" | ") + val1;
  if (!val2.isEmpty()) val += QString(" | ") + val2;
  return val;
}

QVariant SchemaProcItem::editionWhatsThis(int column) const
{
    return "<p>To edit the schema properties, select the schema and use the input panel. <a href=\"modification.html#property-page-for-schema\">More...</a></p>";
}
