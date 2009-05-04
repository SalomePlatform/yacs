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
#include "SceneElementaryNodeItem.hxx"
#include "SceneInPortItem.hxx"
#include "SceneOutPortItem.hxx"
#include "SceneComposedNodeItem.hxx"
#include "guiObservers.hxx"
#include "commandsProc.hxx"
#include "Scene.hxx"
#include "ElementaryNode.hxx"
#include "InputPort.hxx"
#include "OutputPort.hxx"

#include "QtGuiContext.hxx"
#include "Menus.hxx"

#include <cassert>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;
using namespace YACS::ENGINE;
using namespace YACS::HMI;

SceneElementaryNodeItem::SceneElementaryNodeItem(QGraphicsScene *scene, SceneItem *parent,
                                             QString label, Subject *subject)
  : SceneNodeItem(scene, parent, label, subject)
{
  _brushColor   = QColor(189,230,185);
  _hiBrushColor = QColor(209,255,205);
  _penColor     = QColor( 15,180,  0);
  _hiPenColor   = QColor( 11,128,  0);
}

SceneElementaryNodeItem::~SceneElementaryNodeItem()
{
}

//! SceneElementaryNodeItem cannot be resized (only ComposedNodeItem can)
void SceneElementaryNodeItem::setWidth(qreal width)
{
}

//! SceneElementaryNodeItem cannot be resized (only ComposedNodeItem can)
void SceneElementaryNodeItem::setHeight(qreal height)
{
}

void SceneElementaryNodeItem::paint(QPainter *painter,
                          const QStyleOptionGraphicsItem *option,
                          QWidget *widget)
{
  //DEBTRACE("SceneElementaryNodeItem::paint");
  painter->save();
  painter->setBrush(QBrush(Qt::NoBrush));
  painter->setPen(QPen(Qt::NoPen));
  painter->drawRect(QRectF(0, 0, _width, _height));
  painter->setPen(getPenColor());
  painter->setBrush(getBrushColor());
  painter->drawRect(QRectF(_nml, _nml,
                           _width-2*_nml, _height-2*_nml));
  painter->restore();
}

void SceneElementaryNodeItem::update(GuiEvent event, int type, Subject* son)
{
  DEBTRACE("SceneElementaryNodeItem::update "<< eventName(event)<<" "<<type<<" "<<son);
  SceneNodeItem::update(event, type, son);
  SceneItem *item;
  switch (event)
    {
    case YACS::HMI::ADD:
      switch (type)
        {
        case YACS::HMI::INPUTPORT:
        case YACS::HMI::INPUTDATASTREAMPORT:
          item =  new SceneInPortItem(_scene,
                                      this,
                                      son->getName().c_str(),
                                      son);
          autoPosNewPort(item, _inPorts.size());
          _inPorts.push_back(item);
          if (Scene::_autoComputeLinks && !QtGuiContext::getQtCurrent()->isLoading())
            {
              YACS::HMI::SubjectProc* subproc = QtGuiContext::getQtCurrent()->getSubjectProc();
              SceneItem *item = QtGuiContext::getQtCurrent()->_mapOfSceneItem[subproc];
              SceneComposedNodeItem *proc = dynamic_cast<SceneComposedNodeItem*>(item);
              proc->rebuildLinks();
            }
          break;
        case YACS::HMI::OUTPUTPORT:
        case YACS::HMI::OUTPUTDATASTREAMPORT:
          item =  new SceneOutPortItem(_scene,
                                       this,
                                       son->getName().c_str(),
                                       son);
          autoPosNewPort(item, _outPorts.size());
          _outPorts.push_back(item);
          if (Scene::_autoComputeLinks && !QtGuiContext::getQtCurrent()->isLoading())
            {
              YACS::HMI::SubjectProc* subproc = QtGuiContext::getQtCurrent()->getSubjectProc();
              SceneItem *item = QtGuiContext::getQtCurrent()->_mapOfSceneItem[subproc];
              SceneComposedNodeItem *proc = dynamic_cast<SceneComposedNodeItem*>(item);
              proc->rebuildLinks();
            }
           break;
//         default:
//           DEBTRACE("SceneElementaryNodeItem::update() ADD, type not handled:" << type);
        }
      break;
    case YACS::HMI::REMOVE:
    case YACS::HMI::SYNCHRO:
      reorganize();
      break;
//     default:
//       DEBTRACE("SceneElementaryNodeItem::update(), event not handled:" << eventName(event));
    }
}

void SceneElementaryNodeItem::popupMenu(QWidget *caller, const QPoint &globalPos)
{
  ElementaryNodeMenu m;
  m.popupMenu(caller, globalPos);
}

void SceneElementaryNodeItem::reorganize()
{
  DEBTRACE("SceneElementaryNodeItem::reorganize() " << _label.toStdString());

  SubjectNode* snode = dynamic_cast<SubjectNode*>(_subject);
  ElementaryNode* father = dynamic_cast<ElementaryNode*>(snode->getNode());
  YASSERT(father);

  list<InputPort*> plisti = father->getSetOfInputPort();
  list<InputPort*>::iterator iti = plisti.begin();
  int nbPorts = 0;
  for (; iti != plisti.end(); ++iti)
    {
      Subject *sub = QtGuiContext::getQtCurrent()->_mapOfSubjectDataPort[(*iti)];
      SceneItem *item = QtGuiContext::getQtCurrent()->_mapOfSceneItem[sub];
      autoPosNewPort(item, nbPorts);
      nbPorts++;
    }

  list<OutputPort*> plisto = father->getSetOfOutputPort();
  list<OutputPort*>::iterator ito = plisto.begin();
  nbPorts = 0;
  for (; ito != plisto.end(); ++ito)
    {
      Subject *sub = QtGuiContext::getQtCurrent()->_mapOfSubjectDataPort[(*ito)];
      SceneItem *item = QtGuiContext::getQtCurrent()->_mapOfSceneItem[sub];
      autoPosNewPort(item, nbPorts);
      nbPorts++;
    }
}
