
#include "SchemaDirContainersItem.hxx"
#include "SchemaContainerItem.hxx"
#include "SchemaModel.hxx"
#include "QtGuiContext.hxx"
#include "ComponentInstance.hxx"
#include "Container.hxx"

#include <QIcon>
#include <cassert>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;
using namespace YACS::ENGINE;
using namespace YACS::HMI;

SchemaDirContainersItem::SchemaDirContainersItem(SchemaItem *parent, QString label, Subject* subject)
  : SchemaItem::SchemaItem(parent, label, subject)
{
  _itemDeco.replace(YLabel, QIcon("icons:folder_cyan.png"));
  _schemaContItemMap.clear();
  _waitingCompItemMap.clear();
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
  assert(aSComp);
  ComponentInstance* component = aSComp->getComponent();
  assert(component);

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

  assert(_schemaContItemMap.count(contName));
  SchemaContainerItem *sci = _schemaContItemMap[contName];
  aSComp->associateToContainer(static_cast<SubjectContainer*>(sci->getSubject()));
}
