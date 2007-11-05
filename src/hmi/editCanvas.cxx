
#include "editCanvas.h"
#include "guiContext.hxx"

#define _DEVDEBUG_
#include "YacsTrace.hxx"

#include <string>
#include <cassert>

using namespace std;

using namespace YACS;
using namespace YACS::HMI;

#define LX 60
#define LY 40
#define OX 10
#define OY 5
#define PX 6
#define PY 6
#define DX 3
#define DY 3
 
// ----------------------------------------------------------------------------

EditCanvas::EditCanvas( QCanvas* c, QWidget* parent,
                        const char* name, WFlags f) :
  QCanvasView(c,parent,name,f)
{
  //_previousSelected = 0;
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
  QCanvasRectangle(OX,OY,LX,LY,YACS::HMI::GuiContext::getCurrent()->getCanvas())
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
  DEBTRACE("CanvasItem::~CanvasItem");
  QCanvasItem* can = GuiContext::getCurrent()->getSelectedCanvasItem();
  if (can == this)
    GuiContext::getCurrent()->setSelectedCanvasItem(0);
  if (_parent)
    {
      ComposedNodeCanvasItem* cnci = dynamic_cast<ComposedNodeCanvasItem*>(_parent);
      if (cnci) cnci->removeChild(this);
    }
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
//   DEBTRACE("CanvasItem::select: " << isSelected << " " << this);
  QColor col;
  if  (isSelected)
    {
      col = getSelectedColor();
      QCanvasItem* can = GuiContext::getCurrent()->getSelectedCanvasItem();
      if (can)
        {
          CanvasItem* citem = dynamic_cast<CanvasItem*>(can);
//           DEBTRACE("previous: " << citem);
          assert(citem);
          citem->select(false);
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
      assert(parent);
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

void CanvasItem::redraw()
{
  DEBTRACE("CanvasItem::redraw");
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
}

int NodeCanvasItem::getLx()
{
  return LX;
}

int NodeCanvasItem::getLy()
{
  return LY;
}

void NodeCanvasItem::update(GuiEvent event, int type, Subject* son)
{
  DEBTRACE("NodeCanvasItem::update");
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
          DEBTRACE("NodeCanvasItem::update() ADD, type not handled:" << type);
        }
      break;
    default:
      DEBTRACE("NodeCanvasItem::update(), event not handled: << event");
    }
}

void NodeCanvasItem::removeInPort(CanvasItem* inport)
{
  _inPorts.remove(inport);
}

void NodeCanvasItem::removeOutPort(CanvasItem* outport)
{
  _outPorts.remove(outport);
}

void NodeCanvasItem::redraw()
{
//   DEBTRACE(" NodeCanvasItem::redraw() "<<getOfx()<<" "<<getOfy()<<" "<<getLx()<<" "<<getLy());
  QCanvas * canvas = GuiContext::getCurrent()->getCanvas();
  drawNode(getOfx(), getOfy(), getLx(), getLy(), getLevel());
  for (list<CanvasItem*>::iterator it = _inPorts.begin(); it!=_inPorts.end(); ++it)
    (*it)->redraw();
  for (list<CanvasItem*>::iterator it = _outPorts.begin(); it!=_outPorts.end(); ++it)
    (*it)->redraw();
  canvas->update();
}

QColor NodeCanvasItem::getNormalColor()
{
  return QColor(171,167,118);
}

QColor NodeCanvasItem::getSelectedColor()
{
  return QColor(219,213,151);
}

int NodeCanvasItem::getOfxInPort(CanvasItem *child)
{
  int ofx = getOfx() +DX;
  for (list<CanvasItem*>::iterator it = _inPorts.begin(); it!=_inPorts.end(); ++it)
    {
      if ((*it) == static_cast<CanvasItem*>(child)) break;
      else ofx += PX+ DX;
    }
  return ofx;
}

int NodeCanvasItem::getOfxOutPort(CanvasItem *child)
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
  : CanvasItem(parent, label, subject)
{
  _children.clear();
}


ComposedNodeCanvasItem::~ComposedNodeCanvasItem()
{
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
          item =  new NodeCanvasItem(this,
                                     son->getName(),
                                     son);
          _children.push_back(item);
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
 
// ----------------------------------------------------------------------------

InPortCanvasItem::InPortCanvasItem(CanvasItem *parent, QString label, Subject* subject)
  : CanvasItem(parent, label, subject)
{
}

InPortCanvasItem::~InPortCanvasItem()
{
  NodeCanvasItem* nci = dynamic_cast<NodeCanvasItem*>(_parent);
  if(nci) nci->removeInPort(this);
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
      NodeCanvasItem* parent = dynamic_cast<NodeCanvasItem*>(_parent);
      assert(parent);
      ofx = parent->getOfxInPort(this);
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
  DEBTRACE(" InportCanvasItem::redraw() "<<getOfx()<<" "<<getOfy()<<" "<<getLx()<<" "<<getLy());
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
  NodeCanvasItem* nci = dynamic_cast<NodeCanvasItem*>(_parent);
  if(nci) nci->removeOutPort(this);
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
      NodeCanvasItem* parent = dynamic_cast<NodeCanvasItem*>(_parent);
      assert(parent);
      ofx = parent->getOfxOutPort(this);
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
  DEBTRACE(" OutportCanvasItem::redraw() "<<getOfx()<<" "<<getOfy()<<" "<<getLx()<<" "<<getLy());
  QCanvas * canvas = GuiContext::getCurrent()->getCanvas();
  drawNode(getOfx(), getOfy(), getLx(), getLy(), getLevel());
  canvas->update();
}

