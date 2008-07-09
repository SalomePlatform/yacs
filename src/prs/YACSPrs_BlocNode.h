// Copyright (C) 2005  OPEN CASCADE, CEA/DEN, EDF R&D, PRINCIPIA R&D
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
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
#ifndef YACSPRS_BLOCNODE_H
#define YACSPRS_BLOCNODE_H

#include "YACSPrs_ElementaryNode.h"

#include <qcanvas.h>

class SUIT_ResourceMgr;

/*! Presentation for the Bloc node. Implement expanded mode.
 */
class YACSPrs_BlocNode : public YACSPrs_ElementaryNode {
 public:
  typedef enum { Collapsed, Expanded } DisplayMode;

  YACSPrs_BlocNode( SUIT_ResourceMgr*, QCanvas*, YACS::HMI::SubjectNode*,
		    DisplayMode theDisplayMode=Expanded,
		    int theZ=0,
		    int theLeft=0, int theTop=0,
		    int theWidth=0, int theHeight=0);
  virtual ~YACSPrs_BlocNode();

  virtual void update( YACS::HMI::GuiEvent event, int type, YACS::HMI::Subject* son);

  DisplayMode getDisplayMode() const { return myDisplayMode; }

  void setChildren(std::set<YACSPrs_ElementaryNode*>& theChildren);
  std::set<YACSPrs_ElementaryNode*> getChildren() const { return myChildren; }
  
  virtual void setCanvas(QCanvas* theCanvas);

  /* reimplement functions from QxGraph_ActiveItem */
  virtual bool isMoveable() { return true; }
  virtual void beforeMoving() { myZ = z(); setMoving(true); /*setZ(myZ+100);*/ }
  virtual void afterMoving() { setMoving(false); setZ(myZ); canvas()->update(); }
  virtual bool isResizable(QPoint thePoint, int& theCursorType);
  virtual bool isResizing();
  virtual void beforeResizing(int theCursorType);
  virtual void resize(QPoint thePoint);
  virtual void afterResizing();
  virtual void showPopup(QWidget* theParent, QMouseEvent* theEvent, const QPoint& theMousePos = QPoint());

  virtual int rtti() const;

  virtual void setVisible(bool b);

  virtual QPointArray constructAreaPoints(int theW, int theH) const;
  void moveBy(double dx, double dy);
  void setZ(double z);
  void setX(int x);
  void setY(int y);

  virtual void update();
  virtual void updateGates();

  virtual int width() const;
  virtual int height() const;
  void resize(int theWidth, int theHeight);
  void updateHeight();

  int minXContent();
  int maxXContent();

  int minYContent();
  int maxYContent();

  int minWidth() const;
  int minHeight() const;

  virtual int maxWidth() const;
  virtual int maxHeight() const;

  virtual int minX() const;
  virtual int maxX() const;
  
  virtual QRect getRect() const;
  virtual QRect getTitleRect() const;
  virtual QRect getGateRect() const;

  // for constraint nodes' moving inside the Bloc-->
  QRect getAreaRect() const;
  virtual bool checkArea(double dx, double dy, QPoint thePoint=QPoint());
  virtual void setContentMoving(bool moving){myContentMoving=moving;};
  // <--

  virtual QString getToolTipText(const QPoint& theMousePos, QRect& theRect) const;

 protected:
  virtual void drawShape(QPainter& thePainter);
  
  virtual void drawTitle(QPainter& thePainter);
  virtual void drawGate(QPainter& thePainter);
  virtual void drawFrame(QPainter& thePainter);

 private:
  std::set<YACSPrs_ElementaryNode*> myChildren;

  DisplayMode myDisplayMode;

  // myColor here is the color of the rectangle bounds (expanded mode) or node color (collapsed mode)

  // for expanded display mode-->
  
  QColor myBoundColor; // color for dashed bounding box

  int myHeight;

  bool myResizing; // resize or not in the current moment
  bool myContentMoving; // move or not the content of Bloc during resizing

  typedef enum { No=0, Left, Top, Right, Bottom,
                 LeftTop, TopRight, RightBottom, BottomLeft } Direction;
  Direction myResizeDirection; // horiz., vert. or diagonals

  // <--
};

#endif
