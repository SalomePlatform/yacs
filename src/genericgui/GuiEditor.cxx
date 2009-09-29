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
#include <Python.h>
#include "GuiEditor.hxx"
#include "RuntimeSALOME.hxx"
#include "Proc.hxx"
#include "Node.hxx"
#include "ForEachLoop.hxx"
#include "Catalog.hxx"
#include "guiObservers.hxx"
#include "QtGuiContext.hxx"
#include "TypeCode.hxx"
#include "Scene.hxx"
#include "SceneComposedNodeItem.hxx"
#include "SceneLinkItem.hxx"
#include "Catalog.hxx"
#include "ItemMimeData.hxx"
#include "Message.hxx"
#include "Resource.hxx"

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

void GuiEditor::CreateNodeFromCatalog(const ItemMimeData* myData, SubjectComposedNode *cnode,bool createNewComponentInstance)
{
  DEBTRACE("GuiEditor::CreateNodeFromCatalog");
  int nb = myData->getDataSize();
  DEBTRACE(nb);
  for (int i=0; i<nb; i++)
    {
      Catalog* catalog = myData->getCatalog(i);
      string compoName =  myData->getCompo(i);
      string service = myData->getType(i);
      DEBTRACE(compoName << "/" << service);
      _createNode(catalog, cnode, service, compoName,createNewComponentInstance);
    }
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

  YACS::HMI::SubjectComposedNode *cnode = dynamic_cast< YACS::HMI::SubjectComposedNode*>(sub);
  if (!cnode)
    {
      DEBTRACE("GuiEditor::CreateNode : no ComposedNode selected!");
      return;
    }

  _createNode(catalog, cnode, typeNode, "", Resource::COMPONENT_INSTANCE_NEW);
}

void GuiEditor::_createNode(YACS::ENGINE::Catalog* catalog,
                            SubjectComposedNode *cnode,
                            std::string service,
                            std::string compoName,
                            bool createNewComponentInstance)
{
  // --- find a name not used

  string name = service;
  if (name == "PresetNode")
    name = "DataIn";
  Node *node =cnode->getNode();
  ComposedNode *father = dynamic_cast<ComposedNode*>(node);
  YASSERT(father);
  list<Node*> children = father->edGetDirectDescendants();
  bool nameInUse = true;
  while (nameInUse)
    {
      nameInUse = false;
      std::stringstream tryname;
      long newid = GuiContext::getCurrent()->getNewId();
      tryname << name << newid;
      if (newid > 100000) break; 
      for (list<Node*>::iterator it = children.begin(); it != children.end(); ++it)
        {
          if ((*it)->getName() == tryname.str())
            nameInUse = true;
        }
      name = tryname.str();
    }

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
      if (!aSwitch->addNode(catalog, compoName, service, name, createNewComponentInstance, swCase))
        Message mess;
    }
  else if (cnode)
    if (!cnode->addNode(catalog, compoName, service, name, createNewComponentInstance))
      Message mess;
}

void GuiEditor::AddTypeFromCatalog(const ItemMimeData* myData)
{
  DEBTRACE("GuiEditor::AddTypeFromCatalog");
  SubjectProc* sProc = QtGuiContext::getQtCurrent()->getSubjectProc();
  int nb = myData->getDataSize();
  DEBTRACE(nb);
  for (int i=0; i<nb; i++)
    {
      Catalog* catalog = myData->getCatalog(i);
      DEBTRACE("catalog " << catalog);
      string aType = myData->getType(i);
      DEBTRACE(aType);
      sProc->addDataType(catalog, aType);
    }
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

void GuiEditor::CreateForEachLoop(std::string type)
{
  DEBTRACE("GuiEditor::CreateForEachLoop");
  // The ForEachLoop node for datatype type must exist in builtin catalog
  // So  create it in builtin catalog if it does not exist and datatype is loaded in the current Proc
  YACS::ENGINE::Catalog *catalog = YACS::ENGINE::getSALOMERuntime()->getBuiltinCatalog();
  Proc* proc = GuiContext::getCurrent()->getProc();
  std::string typeName="ForEachLoop_"+type;
  if (!catalog->_composednodeMap.count(typeName))
    {
      if(proc->typeMap.count(type))
        {
          catalog->_composednodeMap[typeName]=new ForEachLoop(typeName,proc->typeMap[type]);
        }
    }
  CreateNode(typeName);
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

void GuiEditor::CreateOptimizerLoop()
{
  DEBTRACE("GuiEditor::CreateOptimizerLoop");
  CreateNode("OptimizerLoop");
}

void GuiEditor::CreateContainer()
{
  DEBTRACE("GuiEditor::CreateContainer");
  SubjectProc *sproc = QtGuiContext::getQtCurrent()->getSubjectProc();
  YASSERT(sproc);
  SubjectContainer *scont = 0;
  while (!scont)
    {
      std::stringstream name;
      long newid = GuiContext::getCurrent()->getNewId();
      if (newid > 100000) break; 
      name << "container" << newid;
      scont = sproc->addContainer(name.str());
    }
}

SubjectDataPort* GuiEditor::CreateInputPort(SubjectElementaryNode* seNode, 
                                            std::string name,
                                            YACS::ENGINE::Catalog *catalog,
                                            std::string type,
                                            SubjectDataPort* before)
{
  DEBTRACE("GuiEditor::CreateInputPort");
  SubjectDataPort *sdp = 0;
  if (name.empty())
    {
      std::stringstream aName;
      long newid = 0;
      while (newid < 100000)
        {
          newid = GuiContext::getCurrent()->getNewId();
          aName << "i" << newid;
          try
            {
              seNode->getNode()->getInputPort(aName.str());
            }
          catch(Exception& ex)
            {
              break;
            }
        }
      sdp = seNode->addInputPort(catalog,type, aName.str());
    }
  else
    sdp = seNode->addInputPort(catalog,type, name);
  if (!sdp)
    Message mess;
  return sdp;
}

SubjectDataPort*  GuiEditor::CreateOutputPort(SubjectElementaryNode* seNode, 
                                              std::string name,
                                              YACS::ENGINE::Catalog *catalog,
                                              std::string type,
                                              SubjectDataPort* before)
{
  DEBTRACE("GuiEditor::CreateOutputPort");
  SubjectDataPort *sdp = 0;
  if (name.empty())
    {
      std::stringstream aName;
      long newid = 0;
      while (newid < 100000)
        {
          newid = GuiContext::getCurrent()->getNewId();
          aName << "o" << newid;
          try
            {
              seNode->getNode()->getOutputPort(aName.str());
            }
          catch(Exception& ex)
            {
              break;
            }
        }
      sdp = seNode->addOutputPort(catalog,type, aName.str());
    }
  else
    sdp = seNode->addOutputPort(catalog,type, name);
  if (!sdp)
    Message mess;
  return sdp;
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

void GuiEditor::CutSubject()
{
  DEBTRACE("GuiEditor::CutSubject");
  Subject *sub = QtGuiContext::getQtCurrent()->getSelectedSubject();
  if (!sub)
    {
      Message mess("GuiEditor::Cut : invalid selection!");
      return;
    }
  DEBTRACE(sub->getName());
  QtGuiContext::getQtCurrent()->setSubjectToCut(sub);
}

void GuiEditor::CopySubject()
{
  DEBTRACE("GuiEditor::CopySubject");
  Subject *sub = QtGuiContext::getQtCurrent()->getSelectedSubject();
  if (!sub)
    {
      Message mess("GuiEditor::Copy : invalid selection!");
      return;
    }
  DEBTRACE(sub->getName());
  QtGuiContext::getQtCurrent()->setSubjectToCopy(sub);
}

void GuiEditor::PasteSubject()
{
  DEBTRACE("GuiEditor::PasteSubject");
  Subject *newParent = QtGuiContext::getQtCurrent()->getSelectedSubject();
  if (!newParent)
    {
      Message mess("GuiEditor::Paste : invalid selection!");
      return;
    }
  bool isCut = false;
  Subject *sub = QtGuiContext::getQtCurrent()->getSubjectToPaste(isCut);
  if (!sub)
    {
      Message mess("Nothing to paste");
      return;
    }
  if (SubjectNode *snode = dynamic_cast<SubjectNode*>(sub))
    {
      if (isCut)
        {
          DEBTRACE("cut");
          if (!snode->reparent(newParent))
            Message mess;
        }
      else
        {
          DEBTRACE("copy");
          if (!snode->copy(newParent))
            Message mess;
        }
      return;
    }
  Message mess("Paste not possible for this kind of object");
}

void GuiEditor::rebuildLinks()
{
// --- only global link redraw for now...
  DEBTRACE("GuiEditor::rebuildLinks");
  YACS::HMI::SubjectProc* subproc = QtGuiContext::getQtCurrent()->getSubjectProc();
  SceneItem *item = QtGuiContext::getQtCurrent()->_mapOfSceneItem[subproc];
  SceneComposedNodeItem *proc = dynamic_cast<SceneComposedNodeItem*>(item);
  proc->rebuildLinks();
}

void GuiEditor::arrangeNodes(bool isRecursive)
{
  Subject *sub = QtGuiContext::getQtCurrent()->getSelectedSubject();
  if (!sub)
    {
      DEBTRACE("GuiEditor::arrangeNodes : invalid selection!");
      return;
    }
  DEBTRACE(sub->getName());

  if (! QtGuiContext::getQtCurrent()->_mapOfSceneItem.count(sub))
    {
      DEBTRACE("no scene item corresponding to this subject");
      return;
    }
  SceneItem* item = QtGuiContext::getQtCurrent()->_mapOfSceneItem[sub];
  SceneComposedNodeItem *sci = dynamic_cast<SceneComposedNodeItem*>(item);
  if (! sci)
    {
      DEBTRACE("no scene composed node item corresponding to this subject");
      return;
    }
  sci->arrangeNodes(isRecursive);
//   if (Scene::_autoComputeLinks && !QtGuiContext::getQtCurrent()->isLoading())
//     {
//       YACS::HMI::SubjectProc* subproc = QtGuiContext::getQtCurrent()->getSubjectProc();
//       SceneItem *item = QtGuiContext::getQtCurrent()->_mapOfSceneItem[subproc];
//       SceneComposedNodeItem *proc = dynamic_cast<SceneComposedNodeItem*>(item);
//       proc->rebuildLinks();
//     }
}
