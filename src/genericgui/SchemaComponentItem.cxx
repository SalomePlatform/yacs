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

#include "SchemaComponentItem.hxx"
#include "QtGuiContext.hxx"
#include "SchemaModel.hxx"
#include "SchemaReferenceItem.hxx"
#include "guiObservers.hxx"
#include "Menus.hxx"

#include <QIcon>
#include <cassert>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;
using namespace YACS::HMI;


SchemaComponentItem::SchemaComponentItem(SchemaItem *parent, QString label, Subject* subject)
  : SchemaItem(parent, label, subject)
{
  _itemDeco.replace(YLabel, QIcon("icons:component.png"));
}

void SchemaComponentItem::update(GuiEvent event, int type, Subject* son)
{
  DEBTRACE("SchemaComponentItem::update");
  //SchemaItem::update(event, type, son);
  SchemaModel *model = 0;
  SchemaReferenceItem *item =0;
  switch (event)
    {
    case YACS::HMI::ADDCHILDREF:
      {
        DEBTRACE("ADDCHILDREF ");
        model = QtGuiContext::getQtCurrent()->getSchemaModel();
        SubjectReference *ref = dynamic_cast<SubjectReference*>(son);
        YASSERT(ref);
        SubjectServiceNode *service = dynamic_cast<SubjectServiceNode*>(ref->getReference());
        YASSERT(service);
        YACS::ENGINE::Proc* proc = GuiContext::getCurrent()->getProc();
        string serviceName = proc->getChildName(service->getNode());
        DEBTRACE("ADDCHILDREF " << ref->getReference()->getName());

        int nbsons = childCount();
        model->beginInsertRows(modelIndex(), nbsons, nbsons);
        item = new SchemaReferenceItem(this,
                                       serviceName.c_str(),
                                       son);
        model->endInsertRows();        
      }
      break;

    case YACS::HMI::REMOVECHILDREF:
      {
        DEBTRACE("REMOVECHILDREF ");
      }
      break;

    case YACS::HMI::CUT:
      {
        DEBTRACE("CUT on " << getSubject()->getName());
        SchemaModel *model = QtGuiContext::getQtCurrent()->getSchemaModel();
        SubjectReference *ref = dynamic_cast<SubjectReference*>(son);
        YASSERT(ref);
        DEBTRACE("CUT " << ref->getReference()->getName());
        SchemaItem *toMove = QtGuiContext::getQtCurrent()->_mapOfSchemaItem[ref];

        int position = toMove->row();
        model->beginRemoveRows(modelIndex(), position, position);
        removeChild(toMove);
        model->endRemoveRows();
      }
      break;

    case YACS::HMI::PASTE:
      {
        DEBTRACE("PASTE on " << getSubject()->getName());
        SchemaModel *model = QtGuiContext::getQtCurrent()->getSchemaModel();
        SubjectReference *ref = dynamic_cast<SubjectReference*>(son);
        YASSERT(ref);
        DEBTRACE("PASTE " << ref->getReference()->getName());
        SchemaItem *toPaste = QtGuiContext::getQtCurrent()->_mapOfSchemaItem[ref];

        int nbsons = childCount();
        model->beginInsertRows(modelIndex(), nbsons, nbsons);
        toPaste->reparent(this);
        model->endInsertRows();
      }
      break;


    default:
      ;
    }
}

void SchemaComponentItem::popupMenu(QWidget *caller, const QPoint &globalPos)
{
  ComponentInstanceMenu m;
  m.popupMenu(caller, globalPos);
}

QVariant SchemaComponentItem::editionWhatsThis(int column) const
{
  return "<p>To edit the component instance properties, select the component instance and use the input panel. <a href=\"modification.html#property-page-for-component-instance-definition\">More...</a></p>";
}
