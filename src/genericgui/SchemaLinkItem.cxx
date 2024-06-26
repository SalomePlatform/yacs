// Copyright (C) 2006-2024  CEA, EDF
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

#include "SchemaLinkItem.hxx"
#include "Menus.hxx"
#include "DataStreamPort.hxx"

#include <QIcon>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;
using namespace YACS::HMI;

SchemaLinkItem::SchemaLinkItem(SchemaItem *parent, QString label, Subject* subject)
  : SchemaItem(parent, label, subject)
{
  switch (subject->getType())
    {
    case YACS::HMI::CONTROLLINK: 
      _itemDeco.replace(YLabel, QIcon("icons:control_link.png"));
      break;
    case YACS::HMI::DATALINK: 
      _itemDeco.replace(YLabel, QIcon("icons:data_link.png"));
      if (SubjectLink *slink = dynamic_cast<SubjectLink*>(subject))
        {
          if (dynamic_cast<YACS::ENGINE::DataStreamPort*>(slink->getSubjectOutPort()->getPort()))
            _itemDeco.replace(YLabel, QIcon("icons:stream_link.png"));            
        }
      break;
    default:
      _itemDeco.replace(YLabel, QIcon("icons:new_link.png"));
    }
}

void SchemaLinkItem::update(GuiEvent event, int type, Subject* son)
{
  DEBTRACE("SchemaLinkItem::update");
  SchemaItem::update(event, type, son);
}

void SchemaLinkItem::popupMenu(QWidget *caller, const QPoint &globalPos)
{
  LinkMenu m;
  m.popupMenu(caller, globalPos);
}


QVariant SchemaLinkItem::editionWhatsThis(int column) const
{
return "<p>Links between ports or nodes under that block level appears in this folder.\n" \
  "You can add data links (between ports) and control links (between nodes) " \
  "by drag from origin (out node or out port) to destination (in node or in port) " \
  "either in 2D graphic view or tree view. <a href=\"modification.html#create-a-link\">More...</a></p>";
}
