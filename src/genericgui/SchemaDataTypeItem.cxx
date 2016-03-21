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

#include "SchemaDataTypeItem.hxx"

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;
using namespace YACS::HMI;

SchemaDataTypeItem::SchemaDataTypeItem(SchemaItem *parent, QString label, Subject* subject)
  : SchemaItem(parent, label, subject)
{
}

void SchemaDataTypeItem::update(GuiEvent event, int type, Subject* son)
{
  DEBTRACE("SchemaDataTypeItem::update");
  SchemaItem::update(event, type, son);
}
 
QVariant SchemaDataTypeItem::editionWhatsThis(int column) const
{
return "All the Data types required in this schema must be in this folder.\n" \
  "You can add data types here by import from catalogs "                \
  "(builtin catalog, session catalog, or a catalog built from another schema).\n" \
  "To add a data type, drag a data type item from catalogs panel and drop it here.";
}

