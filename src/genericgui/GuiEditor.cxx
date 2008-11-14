
#include <Python.h>
#include "GuiEditor.hxx"
#include "RuntimeSALOME.hxx"
#include "Proc.hxx"
#include "Node.hxx"
#include "Catalog.hxx"
#include "guiObservers.hxx"
#include "QtGuiContext.hxx"
#include "TypeCode.hxx"
#include "SceneComposedNodeItem.hxx"
#include "SceneLinkItem.hxx"
#include "Catalog.hxx"
#include "ItemMimeData.hxx"

#include <string>
#include <sstream>
#include <iostream>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;
using namespace YACS::ENGINE;
using namespace YACS::HMI;

GuiEditor::GuiEditor()
{
  DEBTRACE("GuiEditor::GuiEditor");
}

GuiEditor::~GuiEditor()
{
  DEBTRACE("GuiEditor::~GuiEditor");
}

void GuiEditor::CreateNodeFromCatalog(const ItemMimeData* myData, SubjectComposedNode *cnode)
{
  DEBTRACE("GuiEditor::CreateNodeFromCatalog");
  Catalog* catalog = myData->getCatalog();
  string compoName =  myData->getCompo();
  string service = myData->getType();
  DEBTRACE(compoName << "/" << service);
  std::stringstream name;
  name << service << GuiContext::getCurrent()->getNewId();
  
  int swCase = 0;
  SubjectSwitch *aSwitch = dynamic_cast<SubjectSwitch*>(cnode);
  if (aSwitch)
    {
      map<int, SubjectNode*> bodyMap = aSwitch->getBodyMap();
      if (bodyMap.empty()) swCase = 1;
      else
        {
          map<int, SubjectNode*>::reverse_iterator rit = bodyMap.rbegin();
          swCase = (*rit).first + 1;
        }
      aSwitch->addNode(catalog, compoName, service, name.str(), swCase);
    }
  else if (cnode) cnode->addNode(catalog, compoName, service, name.str());
}

void GuiEditor::AddTypeFromCatalog(const ItemMimeData* myData)
{
  DEBTRACE("GuiEditor::AddTypeFromCatalog");
  Catalog* catalog = myData->getCatalog();
  DEBTRACE(" --------------------------------------------------------------- catalog " << catalog);
  string aType = myData->getType();
  DEBTRACE(aType);
  SubjectProc* sProc = QtGuiContext::getQtCurrent()->getSubjectProc();
  sProc->addDataType(catalog, aType);
}

void GuiEditor::CreateNode(std::string typeNode)
{
  DEBTRACE("GuiEditor::CreateNode " << typeNode);
  YACS::ENGINE::Catalog *catalog = YACS::ENGINE::getSALOMERuntime()->getBuiltinCatalog();

  Subject *sub = QtGuiContext::getQtCurrent()->getSelectedSubject();
  if (!sub)
    {
      DEBTRACE("GuiEditor::CreateNode : invalid selection!");
      return;
    }
  DEBTRACE(sub->getName());

  YACS::HMI::SubjectComposedNode *subject = dynamic_cast< YACS::HMI::SubjectComposedNode*>(sub);
  if (!subject)
    {
      DEBTRACE("GuiEditor::CreateNode : no ComposedNode selected!");
      return;
    }

  std::stringstream name;
  name << typeNode << GuiContext::getCurrent()->getNewId();

  YACS::HMI::SubjectSwitch *aSwitch = dynamic_cast< YACS::HMI::SubjectSwitch*>(subject);
  if (aSwitch)
    {
      map<int, SubjectNode*> bodyMap = aSwitch->getBodyMap();
      int swCase = 0;
      if (bodyMap.empty()) swCase = 1;
      else
        {
          map<int, SubjectNode*>::reverse_iterator rit = bodyMap.rbegin();
          swCase = (*rit).first + 1;
        }
      aSwitch->addNode(catalog, "", typeNode, name.str(), swCase);
    }
  else
    subject->addNode(catalog, "", typeNode, name.str());
}

void GuiEditor::CreateBloc()
{
  DEBTRACE("GuiEditor::CreateBloc");
  CreateNode("Bloc");
}

void GuiEditor::CreateForLoop()
{
  DEBTRACE("GuiEditor::CreateForLoop");
  CreateNode("ForLoop");
}

void GuiEditor::CreateForEachLoop()
{
  DEBTRACE("GuiEditor::CreateForEachLoop");
  CreateNode("ForEachLoopDouble");
}

void GuiEditor::CreateWhileLoop()
{
  DEBTRACE("GuiEditor::CreateWhileLoop");
  CreateNode("WhileLoop");
}

void GuiEditor::CreateSwitch()
{
  DEBTRACE("GuiEditor::CreateSwitch");
  CreateNode("Switch");
}

void GuiEditor::CreateContainer()
{
  DEBTRACE("GuiEditor::CreateContainer");
  SubjectProc *sproc = QtGuiContext::getQtCurrent()->getSubjectProc();
  assert(sproc);
  std::stringstream name;
  name << "container" << GuiContext::getCurrent()->getNewId();
  sproc->addContainer(name.str());
}

void GuiEditor::CreateInputPort(SubjectElementaryNode* seNode, 
                                std::string name,
                                YACS::ENGINE::Catalog *catalog,
                                std::string type,
                                SubjectDataPort* before)
{
  DEBTRACE("GuiEditor::CreateInputPort");
  //YACS::ENGINE::Catalog *catalog = YACS::ENGINE::getSALOMERuntime()->getBuiltinCatalog();
  //Catalog *catalog = QtGuiContext::getQtCurrent()->getSessionCatalog();

  std::stringstream aName;
  if (name.empty())
    aName << "i" << GuiContext::getCurrent()->getNewId();
  else
    aName << name;
  seNode->addInputPort(catalog,type, aName.str());
}

void GuiEditor::CreateOutputPort(SubjectElementaryNode* seNode, 
                                 std::string name,
                                 YACS::ENGINE::Catalog *catalog,
                                 std::string type,
                                 SubjectDataPort* before)
{
  DEBTRACE("GuiEditor::CreateOutputPort");
  //YACS::ENGINE::Catalog *catalog = YACS::ENGINE::getSALOMERuntime()->getBuiltinCatalog();
  //Catalog *catalog = QtGuiContext::getQtCurrent()->getSessionCatalog();

  std::stringstream aName;
  if (name.empty())
    aName << "o" << GuiContext::getCurrent()->getNewId();
  else
    aName << name;
  seNode->addOutputPort(catalog,type, aName.str());
}

/*!
 * Subject destruction, command from popup menu: needs a valid selection
 */
void GuiEditor::DeleteSubject()
{
  DEBTRACE("GuiEditor::DeleteSubject");
  QModelIndexList selList
    = QtGuiContext::getQtCurrent()->getSelectionModel()->selectedIndexes();
  if (selList.isEmpty())
    return;

  QModelIndex selected = selList.front();
  if (!selected.isValid())
    return;

//   QModelIndex parent = selected.parent();
//   if (!parent.isValid())
//     return;

// //   int position = selected.row();
// //   QtGuiContext::getQtCurrent()->getSchemaModel()->removeRows(position, 1, parent);

//   SchemaItem *parentItem = static_cast<SchemaItem*>(parent.internalPointer());
//   if (!parentItem) return;
//   Subject *subParent = parentItem->getSubject();

  SchemaItem *selItem = static_cast<SchemaItem*>(selected.internalPointer());
  if (!selItem) return;
  Subject *subToRemove = selItem->getSubject();
  Subject *subParent = subToRemove->getParent();

  DeleteSubject(subParent, subToRemove);
}

/*!
 * Subject destruction, command from button in port table view or via selection
 */
void GuiEditor::DeleteSubject(Subject* parent,
                              Subject* toRemove)
{
  DEBTRACE("GuiEditor::DeleteSubject "<<parent->getName()<<" "<<toRemove->getName());
  parent->destroy(toRemove);
}

void GuiEditor::rebuildLinks()
{
//   Subject *sub = QtGuiContext::getQtCurrent()->getSelectedSubject();
//   if (!sub)
//     {
//       DEBTRACE("GuiEditor::CreateNode : invalid selection!");
//       return;
//     }
//   DEBTRACE(sub->getName());

//   if (! QtGuiContext::getQtCurrent()->_mapOfSceneItem.count(sub))
//     {
//       DEBTRACE("no scene item corresponding to this subject");
//       return;
//     }
//   SceneItem* item = QtGuiContext::getQtCurrent()->_mapOfSceneItem[sub];
//   SceneComposedNodeItem *bloc = dynamic_cast<SceneComposedNodeItem*>(item);
//   if (! bloc)
//     {
//       DEBTRACE("no scene composed node item corresponding to this subject");
//       return;
//     }

// --- only global link redraw for now...
  
  YACS::HMI::SubjectProc* subproc = QtGuiContext::getQtCurrent()->getSubjectProc();
  SceneItem *item = QtGuiContext::getQtCurrent()->_mapOfSceneItem[subproc];
  SceneComposedNodeItem *proc = dynamic_cast<SceneComposedNodeItem*>(item);
  proc->rebuildLinks();
}
