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

#include <Python.h>
#include "GuiEditor.hxx"
#include "RuntimeSALOME.hxx"
#include "Proc.hxx"
#include "Node.hxx"
#include "ForEachLoop.hxx"
#include "Catalog.hxx"
#include "Container.hxx"
#include "ComponentInstance.hxx"
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
#include "FormUndoRedo.hxx"
#include <QMessageBox>

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
  // approximative conversion from latin1 to US ascii (removing accentuation)
  DEBTRACE("GuiEditor::GuiEditor");
  _table    = "________________"  ; //   0 -  15
  _table   += "________________"  ; //  16 -  31
  _table   += " !\"#$%&'()*+,-./" ; //  32 -  47
  _table   += "0123456789:;<=>?"  ; //  48 -  63
  _table   += "@ABCDEFGHIJKLMNO"  ; //  64 -  79
  _table   += "PQRSTUVWXYZ[\\]^_" ; //  80 -  95
  _table   += "`abcdefghijklmno"  ; //  96 - 111
  _table   += "pqrstuvwxyz{|}~_"  ; // 112 - 127
  _table   += "________________"  ; // 128 - 143
  _table   += "________________"  ; // 144 - 159
  _table   += "_icLoY|-_ca-__r-" ;  // 160 - 175
  _table   += "-_23'u_..10\"___?" ;  // 176 - 191
  _table   += "AAAAAAACEEEEIIII"  ; // 192 - 207
  _table   += "DNOOOOOx0UUUUYPB"  ; // 208 - 223
  _table   += "aaaaaaaceeeeiiii"  ; // 224 - 239
  _table   += "onooooo-0uuuuypy"  ; // 240 - 255
  //_table[167] = char(167); // '§'
  //_table[176] = char(176); // '°'
  DEBTRACE(_table.size() << " " << _table);
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

SubjectNode* GuiEditor::CreateNode(std::string typeNode)
{
  DEBTRACE("GuiEditor::CreateNode " << typeNode);
  YACS::ENGINE::Catalog *catalog = YACS::ENGINE::getSALOMERuntime()->getBuiltinCatalog();

  Subject *sub = QtGuiContext::getQtCurrent()->getSelectedSubject();
  if (!sub)
    {
      DEBTRACE("GuiEditor::CreateNode : invalid selection!");
      return 0;
    }
  DEBTRACE(sub->getName());

  YACS::HMI::SubjectComposedNode *cnode = dynamic_cast< YACS::HMI::SubjectComposedNode*>(sub);
  if (!cnode)
    {
      DEBTRACE("GuiEditor::CreateNode : no ComposedNode selected!");
      return 0;
    }

  return _createNode(catalog, cnode, typeNode, "", Resource::COMPONENT_INSTANCE_NEW); 
}

SubjectNode* GuiEditor::_createNode(YACS::ENGINE::Catalog* catalog,
				    SubjectComposedNode *cnode,
				    std::string service,
				    std::string compoName,
				    bool createNewComponentInstance)
{
  SubjectNode* aNewNode = 0;

  // --- find a name not used

  string name = service;
  if (name == "PresetNode")
    name = "DataIn";
  Node *node =cnode->getNode();
  ComposedNode *father = dynamic_cast<ComposedNode*>(node);
  YASSERT(father);
  list<Node*> children = father->edGetDirectDescendants();
  bool nameInUse = true;
  std::stringstream tryname;
  while (nameInUse)
    {
      nameInUse = false;
      long newid = GuiContext::getCurrent()->getNewId();
      tryname.str("");
      tryname << name << newid;
      if (newid > 100000) break; 
      for (list<Node*>::iterator it = children.begin(); it != children.end(); ++it)
        {
          if ((*it)->getName() == tryname.str())
            nameInUse = true;
        }
    }
  name = tryname.str();

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
      aNewNode = aSwitch->addNode(catalog, compoName, service, name, createNewComponentInstance, swCase);
      if (!aNewNode)
        Message mess;
    }
  else if (cnode && (dynamic_cast<SubjectBloc*>(cnode) == 0) && cnode->getChild() != 0)
    {
      // loop with a body : can't add a node
      QMessageBox msgBox;
      std::string msg;
      msg="This loop has already a body. It is not possible to add directly another node\n";
      msg=msg+"Do you want to put the existing node in a bloc and add the new node in this bloc ?\n";
      msgBox.setText(msg.c_str());
      msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No );
      msgBox.setDefaultButton(QMessageBox::No);
      int ret = msgBox.exec();
      if(ret == QMessageBox::Yes)
        {
          // User wants to put body node in bloc
          if (cnode->getChild()->putInComposedNode("Bloc1","Bloc"))
            {
              //the bloc has been successfully created. Add the new node
              SubjectBloc* newbloc = dynamic_cast<SubjectBloc*>(cnode->getChild());
	      aNewNode = newbloc->addNode(catalog, compoName, service, name, createNewComponentInstance);
              if (!aNewNode)
                Message mess;
            }
          else
            Message mess;
        }
    }
  else if (cnode)
    aNewNode = cnode->addNode(catalog, compoName, service, name, createNewComponentInstance);
    if (!aNewNode)
      Message mess;
  return aNewNode;
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
  SubjectContainerBase *scont = 0;
  while (!scont)
    {
      std::stringstream name;
      long newid = GuiContext::getCurrent()->getNewId();
      if (newid > 100000) break; 
      name.str("");
      name << "container" << newid;
      scont = sproc->addContainer(name.str());
    }
}

void GuiEditor::CreateHPContainer()
{
  DEBTRACE("GuiEditor::CreateHPContainer");
  SubjectProc *sproc = QtGuiContext::getQtCurrent()->getSubjectProc();
  YASSERT(sproc);
  SubjectContainerBase *scont = 0;
  while (!scont)
    {
      std::stringstream name;
      long newid = GuiContext::getCurrent()->getNewId();
      if (newid > 100000) break;
      name.str("");
      name << "container" << newid;
      scont = sproc->addHPContainer(name.str());
    }
}

void GuiEditor::CreateComponentInstance()
{
  DEBTRACE("GuiEditor::CreateComponentInstance");
  SubjectProc *sproc = QtGuiContext::getQtCurrent()->getSubjectProc();
  YASSERT(sproc);
  Subject *sub = QtGuiContext::getQtCurrent()->getSelectedSubject();
  SubjectComponent *sco = dynamic_cast<SubjectComponent*>(sub);
  if (!sco)
    {
      DEBTRACE("GuiEditor::CreateComponentInstance: " << "selection is not a component");
      return;
    }
  string compoName = sco->getComponent()->getCompoName();
  string containerName = sco->getComponent()->getContainer()->getName();
  sproc->addComponent(compoName, containerName);
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
          aName.str("");
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
          aName.str("");
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
 * Subject shrink or expand, command from popup menu: needs a valid selection
 */
void GuiEditor::shrinkExpand(Qt::KeyboardModifiers kbModifiers) {
  DEBTRACE("GuiEditor::shrinkExpand");

  Subject* sub = QtGuiContext::getQtCurrent()->getSelectedSubject();
  if (!sub) {
    DEBTRACE("GuiEditor::shrinkExpand : invalid selection!");
    return;
  };

  if (! QtGuiContext::getQtCurrent()->_mapOfSceneItem.count(sub)) {
    DEBTRACE("GuiEditor::shrinkExpand: no scene item corresponding to this subject");
    return;
  };

  SceneItem* item = QtGuiContext::getQtCurrent()->_mapOfSceneItem[sub];
  SceneNodeItem *sni = dynamic_cast<SceneNodeItem*>(item);
  if (!sni) {
    DEBTRACE("GuiEditor::shrinkExpand: no scene node item corresponding to this subject");
    return;
  };

  ShrinkMode aShrinkMode = CurrentNode;
  if (kbModifiers == Qt::ControlModifier) {
    aShrinkMode = ElementaryNodes;
  } else if (kbModifiers == (Qt::ShiftModifier|Qt::ControlModifier)) {
    aShrinkMode = ChildrenNodes;
  }

  sni->reorganizeShrinkExpand(aShrinkMode);
  sni->showOutScopeLinks();
  sni->updateLinks();
}

/*!
 * Subject destruction, command from popup menu: needs a valid selection
 */
void GuiEditor::DeleteSubject()
{
  DEBTRACE("GuiEditor::DeleteSubject");
  if (!QtGuiContext::getQtCurrent()->isEdition()) return;
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
  if (!QtGuiContext::getQtCurrent()->isEdition()) return;
  toRemove->askRegisterUndoDestroy();
  if(!parent->destroy(toRemove))
    Message mess;
  // Empty the clipboard in order to avoid the copy of a destroyed object.
  QtGuiContext::getQtCurrent()->setSubjectToCopy(NULL);
}

void GuiEditor::CutSubject()
{
  DEBTRACE("GuiEditor::CutSubject");
  if (!QtGuiContext::getQtCurrent()->isEdition()) return;
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
  if (!QtGuiContext::getQtCurrent()->isEdition()) return;
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
  if (!QtGuiContext::getQtCurrent()->isEdition()) return;
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

void GuiEditor::PutSubjectInBloc()
{
  Subject *sub = QtGuiContext::getQtCurrent()->getSelectedSubject();
  if (!sub)
    {
      Message mess("GuiEditor::PutSubjectInBloc : invalid selection!");
      return;
    }
  if (SubjectNode *snode = dynamic_cast<SubjectNode*>(sub))
    {
      //Build the set of children node names
      Node* node=snode->getNode();
      ComposedNode* father=node->getFather();
      std::list<Node*> children = father->edGetDirectDescendants();
      std::set<std::string> names;
      for (std::list<Node*>::iterator it = children.begin(); it != children.end(); ++it)
        names.insert((*it)->getName());

      std::stringstream tryname;
      long newid=0;
      while (newid < 100000)
	{
	  tryname.str("");
	  tryname << "Bloc" << newid;
	  if(names.find(tryname.str()) == names.end())break;
	  newid++;
	}
      
      if (!snode->putInComposedNode(tryname.str(),"Bloc"))
        Message mess;

      return;
    }
  Message mess("Put in Bloc not possible for this kind of object");
}

void GuiEditor::PutGraphInNode(std::string typeNode)
{
  // put graph in Bloc node before
  std::string blocname = PutGraphInBloc();
  // put the built bloc into target node type
  Proc* proc = GuiContext::getCurrent()->getProc();
  Node* bloc = proc->getChildByShortName(blocname);
  SubjectNode * sbloc = GuiContext::getCurrent()->_mapOfSubjectNode[bloc];
  // create a target node
  SubjectNode * snode = CreateNode(typeNode);
  // put the built bloc into target node
  sbloc->putInComposedNode(snode->getName(), typeNode);
  // select a target node
  YACS::HMI::SubjectProc* subproc = QtGuiContext::getQtCurrent()->getSubjectProc();
  QtGuiContext::getQtCurrent()->setSelectedSubject(subproc);
  arrangeNodes(false);
}

std::string GuiEditor::PutGraphInBloc()
{
  Proc* proc = GuiContext::getCurrent()->getProc();
  std::list<Node *> children = proc->edGetDirectDescendants();

  //get the set of children node names
  std::set<std::string> names;
  for (std::list<Node*>::iterator it = children.begin(); it != children.end(); ++it)
    names.insert((*it)->getName());

  //get the next numbered name
  std::stringstream tryname;
  long newid = GuiContext::getCurrent()->getNewId();
  while (newid < 100000)
    {
      tryname.str("");
      tryname << "Bloc" << newid;
      if(names.find(tryname.str()) == names.end())break;
      newid++;
    }
  std::string blocname = tryname.str();

  //put one by one the child nodes of Proc node into a new Bloc node
  std::map< std::string, std::pair<QPointF, QPointF> > aMapOfNodePosition;
  SceneItem *item = 0;
  SceneNodeItem *inode = 0;
  SubjectNode * snode = 0;
  for (std::list<Node*>::iterator it = children.begin(); it != children.end(); ++it)
    {
      snode = GuiContext::getCurrent()->_mapOfSubjectNode[(*it)];
      snode->saveLinks();
      item = QtGuiContext::getQtCurrent()->_mapOfSceneItem[snode];
      YASSERT(item);
      inode = dynamic_cast<SceneNodeItem*>(item);
      YASSERT(inode);
      // save current node position to restore it after reparenting
      aMapOfNodePosition[snode->getName()] = std::make_pair(inode->pos(), QPointF(inode->getExpandedX(), inode->getExpandedY()));
      // put in Bloc node
      snode->putInComposedNode(blocname, "Bloc", false);
    }
  for (std::list<Node*>::iterator it = children.begin(); it != children.end(); ++it)
    {
      snode = 0;
      snode = GuiContext::getCurrent()->_mapOfSubjectNode[(*it)];
      snode->restoreLinks();
      item = QtGuiContext::getQtCurrent()->_mapOfSceneItem[snode];
      YASSERT(item);
      inode = dynamic_cast<SceneNodeItem*>(item);
      YASSERT(inode);
      // restore node position
      inode->setPos(aMapOfNodePosition[snode->getName()].first);
      // update node position for shrink/expand operation
      inode->setExpandedPos(aMapOfNodePosition[snode->getName()].second);
      // notify node about position changing
      inode->checkGeometryChange();
    }
  Node* bloc = proc->getChildByShortName(blocname);
  SubjectNode* sbloc = GuiContext::getCurrent()->_mapOfSubjectNode[bloc];
  item = QtGuiContext::getQtCurrent()->_mapOfSceneItem[sbloc];
  YASSERT(item);
  // notify bloc about child position changing
  item->checkGeometryChange();
  // select a target bloc
  YACS::HMI::SubjectProc* subproc = QtGuiContext::getQtCurrent()->getSubjectProc();
  QtGuiContext::getQtCurrent()->setSelectedSubject(subproc);
  return blocname;
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

void GuiEditor::arrangeProc()
{
  YACS::HMI::SubjectProc* subproc = QtGuiContext::getQtCurrent()->getSubjectProc();
  SceneItem *item = QtGuiContext::getQtCurrent()->_mapOfSceneItem[subproc];
  SceneComposedNodeItem *proc = dynamic_cast<SceneComposedNodeItem*>(item);
  proc->arrangeNodes(true);
}

void GuiEditor::showUndo(QWidget *parent)
{
  FormUndoRedo *undo = new FormUndoRedo(parent);
  undo->tabWidget->setCurrentWidget(undo->undoTab);
  undo->exec();
}

void GuiEditor::showRedo(QWidget *parent)
{
  FormUndoRedo *redo = new FormUndoRedo(parent);
  redo->tabWidget->setCurrentWidget(redo->redoTab);
  redo->exec();
}

/*! Replace accentuated characters from latin1 to US ascii equivalent without accent.
*   I did not found anything to do that in Qt...
*/
QString GuiEditor::asciiFilter(const QString & name)
{
  DEBTRACE(name.toStdString());
  string aName = name.toLatin1().data();
  DEBTRACE(aName);
  for (int i=0; i < aName.size(); i++)
    {
      int v = (unsigned char)(aName[i]);
      DEBTRACE(v << " " << _table[v]);
      aName[i] = _table[v];
    }
  DEBTRACE(aName);
  return aName.c_str();
}
