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

#include "VisitorSaveGuiSchema.hxx"
#include "Proc.hxx"
#include "QtGuiContext.hxx"
#include "guiObservers.hxx"
#include "SceneItem.hxx"
#include "SceneNodeItem.hxx"

#include <cassert>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;
using namespace YACS::HMI;
using namespace YACS::ENGINE;

VisitorSaveGuiSchema::VisitorSaveGuiSchema(YACS::ENGINE::Proc* proc)
  : VisitorSaveSalomeSchema(proc), _proc(proc)
{
  DEBTRACE("VisitorSaveGuiSchema::VisitorSaveGuiSchema");
}

VisitorSaveGuiSchema::~VisitorSaveGuiSchema()
{
  DEBTRACE("VisitorSaveGuiSchema::~VisitorSaveGuiSchema");
}

void VisitorSaveGuiSchema::visitProc()
{
  DEBTRACE("VisitorSaveGuiSchema::visitProc");
  VisitorSaveSalomeSchema::visitProc(_proc);
  writePresentation();
}

void VisitorSaveGuiSchema::writePresentation()
{
  DEBTRACE("VisitorSaveGuiSchema::writePresentation");
  set<Node*> nodeSet = getAllNodes(_proc);

  for (set<Node*>::iterator iter = nodeSet.begin(); iter != nodeSet.end(); ++iter)
  {
    Node* node = *iter;
    SubjectNode * snode = QtGuiContext::getQtCurrent()->_mapOfSubjectNode[node];
    SceneItem* item = QtGuiContext::getQtCurrent()->_mapOfSceneItem[snode];
    YASSERT(item);
    SceneNodeItem* inode = dynamic_cast<SceneNodeItem*>(item);
    YASSERT(inode);
    writeItem(_proc->getChildName(node), inode);
  }

  SubjectNode * sproc = QtGuiContext::getQtCurrent()->getSubjectProc();
  SceneItem* item = QtGuiContext::getQtCurrent()->_mapOfSceneItem[sproc];
  YASSERT(item);
  SceneNodeItem* inode = dynamic_cast<SceneNodeItem*>(item);
  YASSERT(inode);
  writeItem("__ROOT__", inode);
}

 void VisitorSaveGuiSchema::writeItem(std::string name, SceneNodeItem* item)
{
  int depth = 1;
  _out << indent(depth) << "<presentation";
  _out                  << " name=\""       << name                      << "\"";
  _out                  << " x=\""          << item->x()                 << "\"";
  _out                  << " y=\""          << item->y()                 << "\"";
  _out                  << " width=\""      << item->getWidth()          << "\"";
  _out                  << " height=\""     << item->getHeight()         << "\"";
  _out                  << " expanded=\""   << item->isExpanded()        << "\"";
  _out                  << " expx=\""       << item->getExpandedX()      << "\"";
  _out                  << " expy=\""       << item->getExpandedY()      << "\"";
  _out                  << " expWidth=\""   << item->getExpandedWidth()  << "\"";
  _out                  << " expHeight=\""  << item->getExpandedHeight() << "\"";
  _out                  << " shownState=\"" << item->getShownState()     << "\"";
  _out                  << "/>" << endl;
}
