//  Copyright (C) 2006-2008  CEA/DEN, EDF R&D
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
#include "SchemaProcItem.hxx"
#include "Menus.hxx"
#include "QtGuiContext.hxx"

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
    case YACS::NOTYETINITIALIZED: sc.setHsv( 45, 50, 255); stateDef = "Not Yet Initialized"; break;
    case YACS::INITIALISED:       sc.setHsv( 90, 50, 255); stateDef = "Initialized";         break;
    case YACS::RUNNING:           sc.setHsv(135, 50, 255); stateDef = "Running";             break;
    case YACS::WAITINGTASKS:      sc.setHsv(180, 50, 255); stateDef = "Waiting Tasks";       break;
    case YACS::PAUSED:            sc.setHsv(225, 50, 255); stateDef = "Paused";              break;
    case YACS::FINISHED:          sc.setHsv(270, 50, 255); stateDef = "Finished";            break;
    case YACS::STOPPED:           sc.setHsv(315, 50, 255); stateDef = "Stopped";             break;
    default:                      sc.setHsv(360, 50, 255); stateDef = "Unknown Status";
   }
  _itemData.replace(YState, stateDef);
  _itemForeground.replace(YState, Qt::darkBlue);
  _itemBackground.replace(YState, sc);
}
