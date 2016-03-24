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

#include "SchemaDirContainersItem.hxx"
#include "SchemaContainerItem.hxx"
#include "SchemaModel.hxx"
#include "QtGuiContext.hxx"
#include "ComponentInstance.hxx"
#include "Container.hxx"
#include "Menus.hxx"

#include <QIcon>
#include <cassert>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;
using namespace YACS::ENGINE;
using namespace YACS::HMI;

SchemaDirContainersItem::SchemaDirContainersItem(SchemaItem *parent, QString label, Subject* subject)
  : SchemaItem(parent, label, subject)
{
  _itemDeco.replace(YLabel, QIcon("icons:folder_cyan.png"));
  _schemaContItemMap.clear();
  _waitingCompItemMap.clear();
}

void SchemaDirContainersItem::popupMenu(QWidget *caller, const QPoint &globalPos)
{
  ContainerDirMenu m;
  m.popupMenu(caller, globalPos);
}



/*! When loading a schema, a container may appear after a component using this container.
 *  After creating the SchemaContainerItem, check if there are SchemaComponentItems to
 *  to create as children of this SchemaContainerItem
 */
void SchemaDirContainersItem::addContainerItem(Subject* subject)
{
  DEBTRACE("SchemaDirContainersItem::addContainerItem");
  SchemaModel *model = QtGuiContext::getQtCurrent()->getSchemaModel();
  int nbsons = childCount();
  string contName = subject->getName();

  model->beginInsertRows(modelIndex(), nbsons, nbsons);
  SchemaContainerItem *item = new SchemaContainerItem(this,
                                                      contName.c_str(),
                                                      subject);
  model->endInsertRows();

  _schemaContItemMap[contName] = item;
}

/*! If a component is not associated to a container, it will be associated to a default container.
 *  If this default container does not exist, it is created.
 *  When loading a schema, container must be created before component: @see SubjectProc::loadProc
 */
void SchemaDirContainersItem::addComponentItem(Subject* subject)
{
  DEBTRACE("SchemaDirContainersItem::addComponentItem");
  SubjectComponent *aSComp = dynamic_cast<SubjectComponent*>(subject);
  YASSERT(aSComp);
  ComponentInstance* component = aSComp->getComponent();
  YASSERT(component);

  string contName = "DefaultContainer";
  Container *container = component->getContainer();
  if (container)
    contName = container->getName();
  else
    {
      if (!_schemaContItemMap.count(contName)) // no container and default container not created
        {
          SubjectProc* sProc = QtGuiContext::getQtCurrent()->getSubjectProc();
          sProc->addContainer(contName);
        }
    }

  YASSERT(_schemaContItemMap.count(contName));
  SchemaContainerItem *sci = _schemaContItemMap[contName];
  aSComp->associateToContainer(static_cast<SubjectContainer*>(sci->getSubject()));
}

QVariant SchemaDirContainersItem::editionWhatsThis(int column) const
{
  return "<p>Containers used in this schema appears in this folder.\n" \
      "You can add containers by using the context menu (entry \"Create container\") " \
      "and then edit its properties in the input panel. <a href=\"modification.html#property-page-for-container\">More...</a></p>";
}
