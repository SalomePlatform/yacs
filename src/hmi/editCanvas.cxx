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
#include "editCanvas.h"
#include "guiContext.hxx"

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

#include <string>
#include <cassert>

using namespace std;

using namespace YACS;
using namespace YACS::HMI;

#define LX 60
#define LY 40
#define OX 20
#define OY 5
#define PX 6
#define PY 6
#define DX 3
#define DY 3
 
// ----------------------------------------------------------------------------

EditCanvas::EditCanvas( YACS::HMI::Subject *context,
                        QCanvas* c, QWidget* parent,
                        const char* name, WFlags f)
  : GuiObserver(), _context(context), QCanvasView(c,parent,name,f)
{
  _context->attach(this);
  //_previousSelected = 0;
}

void EditCanvas::update(GuiEvent event, int type, Subject* son)
{
  DEBTRACE("editCanvas::update");
  switch (event)
    {
    case YACS::HMI::NEWROOT:
      setNewRoot(son);
      break;
    default:
      DEBTRACE("editCanvas::update(), event not handled: "<< event);
    }
}

void EditCanvas::setNewRoot(YACS::HMI::Subject *root)
{
  YACS::ENGINE::Proc* proc = GuiContext::getCurrent()->getProc();
//   SubjectProc* subjectProc =GuiContext::getCurrent()->getSubjectProc();
  _rootItem = new ComposedNodeCanvasItem(0, proc->getName(), root);
}

void EditCanvas::contentsMousePressEvent(QMouseEvent* e)
{
  QCanvasItemList l = canvas()->collisions(e->pos());
  double zmax = - 1.E9;
  QCanvasItem *item = 0;
  for (QCanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it)
    {
//       if ( (*it)->rtti() == QCanvasRectangle::RTTI )
//         DEBTRACE("A QCanvasRectangle lies somewhere at this point");
      if ((*it)->z() > zmax)
        {
          zmax = (*it)->z();
          item = *it;
        }
    }
  if (item)
    {
      CanvasItem* citem = dynamic_cast<CanvasItem*>(item);
      if (citem) 
          citem->getSubject()->select(true);
    }
}

// ----------------------------------------------------------------------------

CanvasItem::CanvasItem(CanvasItem *parent, QString label, Subject* subject) :
  GuiObserver(), QCanvasRectangle(OX,OY,LX,LY,YACS::HMI::GuiContext::getCurrent()->getCanvas())
{
  _parent = parent;
  _label = label;
  _subject = subject;
  _subject->attach(this);
  _level = 1;
  _root = this;
  if (_parent) 
    {
      _level = _parent->getLevel() +1;
      _root = _parent->getRoot();
    }
  _ofx = OX; _ofy = OY; _lx = LX; _ly = LY;
}

CanvasItem::~CanvasItem()
{
  DEBTRACE("CanvasItem::~CanvasItem " << _subject->getName());
  QCanvasItem* can = GuiContext::getCurrent()->getSelectedCanvasItem();
  if (can == this)
    GuiContext::getCurrent()->setSelectedCanvasItem(0);
//   QCanvas * canvas = GuiContext::getCurrent()->getCanvas();
//   canvas->update();
}

CanvasItem *CanvasItem::getParent()
{
  return _parent;
}

CanvasItem *CanvasItem::getRoot()
{
  return _root;
}

void CanvasItem::select(bool isSelected)
{
  DEBTRACE("CanvasItem::select: " << isSelected << " " << this);
  QColor col;
  if  (isSelected)
    {
      col = getSelectedColor();
      QCanvasItem* can = GuiContext::getCurrent()->getSelectedCanvasItem();
      if (can)
        {
          CanvasItem* citem = dynamic_cast<CanvasItem*>(can);
          DEBTRACE("previous: " << citem);
          if (citem != this)
            {
              YASSERT(citem);
              citem->select(false);
            }
        }
      GuiContext::getCurrent()->setSelectedCanvasItem(this);
    }
  else 
    col = getNormalColor();
   setBrush(col);
  show();
  QCanvas * canvas = GuiContext::getCurrent()->getCanvas();
  canvas->update();
}

QColor CanvasItem::getNormalColor()
{
  return QColor(180,180,180);
}

QColor CanvasItem::getSelectedColor()
{
  return QColor(215,215,215);
}

Subject* CanvasItem::getSubject()
{
  return _subject;
}

int CanvasItem::getLevel()
{
  return _level;
}

int CanvasItem::getOfx()
{
  _ofx = (_level +1)*OX;
  return _ofx;
}

int CanvasItem::getOfy()
{
  int oy = OY;
  if (_parent)
    {
      ComposedNodeCanvasItem* parent = dynamic_cast<ComposedNodeCanvasItem*>(_parent);
      YASSERT(parent);
      oy = parent->getOfyChild(this);
    }
  _ofy = oy;
  return _ofy;
}

int CanvasItem::getLx()
{
  return _lx;
}

int CanvasItem::getLy()
{
  return _ly;
}

int CanvasItem::getOfxInPort(CanvasItem *child)
{
  return getOfx() +DX; // used only in derivation
}

int CanvasItem::getOfxOutPort(CanvasItem *child)
{
  return getOfy() +DY; // used only in derivation
}

void CanvasItem::redraw()
{
//   DEBTRACE("CanvasItem::redraw");
}

void CanvasItem::drawNode(int ofx, int ofy, int lx, int ly, int level)
{
  setBrush(getNormalColor());
  setPen( QPen(Qt::black, 2) );
  setSize(lx,ly);
  setX(ofx);
  setY(ofy);
  setZ(level);
  show();
}

// ----------------------------------------------------------------------------

NodeCanvasItem::NodeCanvasItem(CanvasItem *parent, QString label, Subject* subject)
  : CanvasItem(parent, label, subject)
{
  _inPorts.clear();
  _outPorts.clear();
}

NodeCanvasItem::~NodeCanvasItem()
{
  DEBTRACE("NodeCanvasItem::~NodeCanvasItem " << _subject->getName());
}

void NodeCanvasItem::removeInPort(CanvasItem* inport)
{
  _inPorts.remove(inport);
}

void NodeCanvasItem::removeOutPort(CanvasItem* outport)
{
  _outPorts.remove(outport);
}

// ----------------------------------------------------------------------------

ElementaryNodeCanvasItem::ElementaryNodeCanvasItem(CanvasItem *parent, QString label, Subject* subject)
  : NodeCanvasItem(parent, label, subject)
{
}

ElementaryNodeCanvasItem::~ElementaryNodeCanvasItem()
{
  DEBTRACE("ElementaryNodeCanvasItem::~ElementaryNodeCanvasItem " << _subject->getName());
  if (_parent)
    {
      ComposedNodeCanvasItem* cnci = dynamic_cast<ComposedNodeCanvasItem*>(_parent);
      if (cnci) cnci->removeChild(this);
    }
}

int ElementaryNodeCanvasItem::getLx()
{
  return LX;
}

int ElementaryNodeCanvasItem::getLy()
{
  return LY;
}

void ElementaryNodeCanvasItem::update(GuiEvent event, int type, Subject* son)
{
  DEBTRACE("ElementaryNodeCanvasItem::update");
  CanvasItem *item;
  switch (event)
    {
    case YACS::HMI::ADD:
      switch (type)
        {
        case YACS::HMI::INPUTPORT:
        case YACS::HMI::INPUTDATASTREAMPORT:
          item =  new InPortCanvasItem(this,
                                       son->getName(),
                                       son);
          _inPorts.push_back(item);
          _root->redraw();
          break;
        case YACS::HMI::OUTPUTPORT:
        case YACS::HMI::OUTPUTDATASTREAMPORT:
          item =  new OutPortCanvasItem(this,
                                        son->getName(),
                                        son);
          _outPorts.push_back(item);
          _root->redraw();
           break;
        default:
          DEBTRACE("ElementaryNodeCanvasItem::update() ADD, type not handled:" << type);
        }
      break;
    default:
      DEBTRACE("ElementaryNodeCanvasItem::update(), event not handled: << event");
    }
}

void ElementaryNodeCanvasItem::redraw()
{
//   DEBTRACE(" ElementaryNodeCanvasItem::redraw() "<<getOfx()<<" "<<getOfy()<<" "<<getLx()<<" "<<getLy());
  QCanvas * canvas = GuiContext::getCurrent()->getCanvas();
  drawNode(getOfx(), getOfy(), getLx(), getLy(), getLevel());
  for (list<CanvasItem*>::iterator it = _inPorts.begin(); it!=_inPorts.end(); ++it)
    (*it)->redraw();
  for (list<CanvasItem*>::iterator it = _outPorts.begin(); it!=_outPorts.end(); ++it)
    (*it)->redraw();
  canvas->update();
}

QColor ElementaryNodeCanvasItem::getNormalColor()
{
  return QColor(171,167,118);
}

QColor ElementaryNodeCanvasItem::getSelectedColor()
{
  return QColor(219,213,151);
}

int ElementaryNodeCanvasItem::getOfxInPort(CanvasItem *child)
{
  int ofx = getOfx() +DX;
  for (list<CanvasItem*>::iterator it = _inPorts.begin(); it!=_inPorts.end(); ++it)
    {
      if ((*it) == static_cast<CanvasItem*>(child)) break;
      else ofx += PX+ DX;
    }
  return ofx;
}

int ElementaryNodeCanvasItem::getOfxOutPort(CanvasItem *child)
{
  int ofx = getOfx() +DX;
  for (list<CanvasItem*>::iterator it = _outPorts.begin(); it!=_outPorts.end(); ++it)
    {
      if ((*it) == static_cast<CanvasItem*>(child)) break;
      else ofx += PX+ DX;
    }
  return ofx;
}
 
// ----------------------------------------------------------------------------

ComposedNodeCanvasItem::ComposedNodeCanvasItem(CanvasItem *parent, QString label, Subject* subject)
  : NodeCanvasItem(parent, label, subject)
{
  _children.clear();
}


ComposedNodeCanvasItem::~ComposedNodeCanvasItem()
{
  DEBTRACE("ComposedNodeCanvasItem::~ComposedNodeCanvasItem " << _subject->getName());
  if (_parent)
    {
      ComposedNodeCanvasItem* cnci = dynamic_cast<ComposedNodeCanvasItem*>(_parent);
      if (cnci) cnci->removeChild(this);
    }
}

int ComposedNodeCanvasItem::getLx()
{
  int lx = 0;
  for (list<CanvasItem*>::iterator it = _children.begin(); it!=_children.end(); ++it)
    {
      int val = (*it)->getLx();
      if (val > lx) lx = val;
    }
  if (lx == 0) lx = LX;
    lx += 2*OX;
    _lx = lx;
  return _lx;
}

int ComposedNodeCanvasItem::getLy()
{
  int ly = 0;
  for (list<CanvasItem*>::iterator it = _children.begin(); it!=_children.end(); ++it)
    ly += (*it)->getLy() + OY;
  if (ly == 0) ly = LY + OY;
  ly += OY;
  _ly = ly;
  return _ly;
}

int ComposedNodeCanvasItem::getOfyChild(CanvasItem *child)
{
  int oy = getOfy();
  for (list<CanvasItem*>::iterator it = _children.begin(); it!=_children.end(); ++it)
    {
      if ((*it) == static_cast<CanvasItem*>(child)) break;
      else oy += (*it)->getLy() + OY;
    }
  oy += OY;
  return oy;
}
  

void ComposedNodeCanvasItem::update(GuiEvent event, int type, Subject* son)
{
  DEBTRACE("ComposedNodeCanvasItem::update");
  CanvasItem *item;
  switch (event)
    {
    case YACS::HMI::ADD:
      switch (type)
        {
        case YACS::HMI::BLOC:
        case YACS::HMI::FORLOOP:
        case YACS::HMI::WHILELOOP:
        case YACS::HMI::SWITCH:
        case YACS::HMI::FOREACHLOOP:
        case YACS::HMI::OPTIMIZERLOOP:
          item =  new ComposedNodeCanvasItem(this,
                                             son->getName(),
                                             son);
          _children.push_back(item);
          _root->redraw();
          break;
        case YACS::HMI::PYTHONNODE:
        case YACS::HMI::PYFUNCNODE:
        case YACS::HMI::CORBANODE:
        case YACS::HMI::CPPNODE:
        case YACS::HMI::SALOMENODE:
        case YACS::HMI::SALOMEPYTHONNODE:
        case YACS::HMI::XMLNODE:
        case YACS::HMI::PRESETNODE:
        case YACS::HMI::OUTNODE:
        case YACS::HMI::STUDYINNODE:
        case YACS::HMI::STUDYOUTNODE:
          item =  new ElementaryNodeCanvasItem(this,
                                     son->getName(),
                                     son);
          _children.push_back(item);
          _root->redraw();
          break;
        case YACS::HMI::INPUTPORT:
        case YACS::HMI::INPUTDATASTREAMPORT:
          item =  new InPortCanvasItem(this,
                                       son->getName(),
                                       son);
          _inPorts.push_back(item);
          _root->redraw();
          break;
        case YACS::HMI::OUTPUTPORT:
        case YACS::HMI::OUTPUTDATASTREAMPORT:
          item =  new OutPortCanvasItem(this,
                                        son->getName(),
                                        son);
          _outPorts.push_back(item);
          _root->redraw();
           break;
        default:
          DEBTRACE("ComposedNodeCanvasItem::update() ADD, type not handled:" << type);
        }
      break;
    default:
      DEBTRACE("ComposedNodeCanvasItem::update(), event not handled: << event");
    }
}

void ComposedNodeCanvasItem::removeChild(CanvasItem* child)
{
  _children.remove(child);
}

void ComposedNodeCanvasItem::redraw()
{
//   DEBTRACE(" ComposedNodeCanvasItem::redraw() "<<getOfx()<<" "<<getOfy()<<" "<<getLx()<<" "<<getLy());
  QCanvas * canvas = GuiContext::getCurrent()->getCanvas();
  drawNode(getOfx(), getOfy(), getLx(), getLy(), getLevel());
  for (list<CanvasItem*>::iterator it = _children.begin(); it!=_children.end(); ++it)
    (*it)->redraw();
  for (list<CanvasItem*>::iterator it = _inPorts.begin(); it!=_inPorts.end(); ++it)
    (*it)->redraw();
  for (list<CanvasItem*>::iterator it = _outPorts.begin(); it!=_outPorts.end(); ++it)
    (*it)->redraw();
  canvas->update();
}

QColor ComposedNodeCanvasItem::getNormalColor()
{
  return QColor(130,177,149);
}

QColor ComposedNodeCanvasItem::getSelectedColor()
{
  return QColor(166,227,191);
}

int ComposedNodeCanvasItem::getOfxInPort(CanvasItem *child)
{
  int ofx = getOfx() +DX;
  for (list<CanvasItem*>::iterator it = _inPorts.begin(); it!=_inPorts.end(); ++it)
    {
      if ((*it) == static_cast<CanvasItem*>(child)) break;
      else ofx += PX+ DX;
    }
  return ofx;
}

int ComposedNodeCanvasItem::getOfxOutPort(CanvasItem *child)
{
  int ofx = getOfx() +DX;
  for (list<CanvasItem*>::iterator it = _outPorts.begin(); it!=_outPorts.end(); ++it)
    {
      if ((*it) == static_cast<CanvasItem*>(child)) break;
      else ofx += PX+ DX;
    }
  return ofx;
}
 
// ----------------------------------------------------------------------------

InPortCanvasItem::InPortCanvasItem(CanvasItem *parent, QString label, Subject* subject)
  : CanvasItem(parent, label, subject)
{
}

InPortCanvasItem::~InPortCanvasItem()
{
  DEBTRACE("InPortCanvasItem::~InPortCanvasItem " << _subject->getName());
  if(_parent)
    {
      NodeCanvasItem* nci = dynamic_cast<NodeCanvasItem*>(_parent);
      if(nci) nci->removeInPort(this);
    }
}

QColor InPortCanvasItem::getNormalColor()
{
  return QColor(144,37,37);
}

QColor InPortCanvasItem::getSelectedColor()
{
  return QColor(255,66,66);
}

void InPortCanvasItem::update(GuiEvent event, int type, Subject* son)
{
  DEBTRACE("InPortCanvasItem::update");
}

int InPortCanvasItem::getLx()
{
  return PX;
}

int InPortCanvasItem::getLy()
{
  return PY;
}

int InPortCanvasItem::getOfx()
{
  int ofx = 0;
  if (_parent)
    {
      ofx = _parent->getOfxInPort(this);
    }
  _ofx = ofx;
  return _ofx;
}

int InPortCanvasItem::getOfy()
{
  int ofy = 0;
  if (_parent)
    ofy = _parent->getOfy() + DY;
  _ofy = ofy;
  return _ofy;
} 

void InPortCanvasItem::redraw()
{
//   DEBTRACE(" InportCanvasItem::redraw() "<<getOfx()<<" "<<getOfy()<<" "<<getLx()<<" "<<getLy());
  QCanvas * canvas = GuiContext::getCurrent()->getCanvas();
  drawNode(getOfx(), getOfy(), getLx(), getLy(), getLevel());
  canvas->update();
}

// ----------------------------------------------------------------------------

OutPortCanvasItem::OutPortCanvasItem(CanvasItem *parent, QString label, Subject* subject)
  : CanvasItem(parent, label, subject)
{
}

OutPortCanvasItem::~OutPortCanvasItem()
{
  DEBTRACE("OutPortCanvasItem::~OutPortCanvasItem " << _subject->getName());
  if(_parent)
    {
      NodeCanvasItem* nci = dynamic_cast<NodeCanvasItem*>(_parent);
      if(nci) nci->removeOutPort(this);
    }
}

QColor OutPortCanvasItem::getNormalColor()
{
  return QColor(144,37,37);
}

QColor OutPortCanvasItem::getSelectedColor()
{
  return QColor(255,66,66);
}

void OutPortCanvasItem::update(GuiEvent event, int type, Subject* son)
{
  DEBTRACE("OutPortCanvasItem::update");
}

int OutPortCanvasItem::getLx()
{
  return PX;
}

int OutPortCanvasItem::getLy()
{
  return PY;
}

int OutPortCanvasItem::getOfx()
{
  int ofx = 0;
  if (_parent)
    {
      ofx = _parent->getOfxOutPort(this);
    }
  _ofx = ofx;
  return _ofx;
}

int OutPortCanvasItem::getOfy()
{
  int ofy = 0;
  if (_parent)
    ofy = _parent->getOfy() + LY -(PY + DY);
  _ofy = ofy;
  return _ofy;
} 

void OutPortCanvasItem::redraw()
{
//   DEBTRACE(" OutportCanvasItem::redraw() "<<getOfx()<<" "<<getOfy()<<" "<<getLx()<<" "<<getLy());
  QCanvas * canvas = GuiContext::getCurrent()->getCanvas();
  drawNode(getOfx(), getOfy(), getLx(), getLy(), getLevel());
  canvas->update();
}

