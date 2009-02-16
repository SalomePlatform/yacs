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
#include "SchemaContainerItem.hxx"
#include "QtGuiContext.hxx"
#include "SchemaModel.hxx"
#include "SchemaComponentItem.hxx"

#include <QIcon>
#include <cassert>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;
using namespace YACS::HMI;

SchemaContainerItem::SchemaContainerItem(SchemaItem *parent, QString label, Subject* subject)
  : SchemaItem::SchemaItem(parent, label, subject)
{
  DEBTRACE("SchemaContainerItem::SchemaContainerItem " << subject->getName());
  SubjectContainer *scont = dynamic_cast<SubjectContainer*>(subject);
  DEBTRACE(scont);
  _itemDeco.replace(YLabel, QIcon("icons:container.png"));
}

void SchemaContainerItem::update(GuiEvent event, int type, Subject* son)
{
  DEBTRACE("SchemaContainerItem::update " << eventName(event) << " " << son->getName());
  //SchemaItem::update(event, type, son);
  switch (event)
    {
    case YACS::HMI::ADDCHILDREF:
      {
        DEBTRACE("ADDCHILDREF ");
        SchemaModel *model = QtGuiContext::getQtCurrent()->getSchemaModel();
        SubjectReference *ref = dynamic_cast<SubjectReference*>(son);
        assert(ref);
        DEBTRACE("ADDCHILDREF " << ref->getReference()->getName());
        addComponentInstance(ref->getReference());
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
        assert(ref);
        DEBTRACE("CUT " << ref->getReference()->getName());
        SchemaItem *toMove = QtGuiContext::getQtCurrent()->_mapOfSchemaItem[ref->getReference()];

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
        assert(ref);
        DEBTRACE("PASTE " << ref->getReference()->getName());
        SchemaItem *toPaste = QtGuiContext::getQtCurrent()->_mapOfSchemaItem[ref->getReference()];

        int nbsons = childCount();
        model->beginInsertRows(modelIndex(), nbsons, nbsons);
        toPaste->reparent(this);
        model->endInsertRows();
      }
      break;

    default:
      SchemaItem::update(event, type, son);
      ;
    }
}

void SchemaContainerItem::addComponentInstance(Subject* subject)
{
  DEBTRACE("SchemaContainersItem::addComponentInstance");
  SchemaModel *model = QtGuiContext::getQtCurrent()->getSchemaModel();
  int nbsons = childCount();
  model->beginInsertRows(modelIndex(), nbsons, nbsons);
  SchemaComponentItem *item = new SchemaComponentItem(this,
                                                      subject->getName().c_str(),
                                                      subject);
  model->endInsertRows();
}