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

#ifndef YACSPRS_LINK_H
#define YACSPRS_LINK_H

#include <guiObservers.hxx>

#include "QxGraph_ActiveItem.h"

#include <qcanvas.h>
#include <qobject.h>

class SUIT_ResourceMgr;

class YACSPrs_Port;
class YACSPrs_InOutPort;
class YACSPrs_LabelPort;
class YACSPrs_ElementaryNode;

class YACSPrs_Link : public QObject {
 Q_OBJECT

 public:
  YACSPrs_Link( SUIT_ResourceMgr*, QCanvas* theCanvas);
  virtual ~YACSPrs_Link();
  
  void show();
  void hide();
  void merge();

  void setCanvas(QCanvas* theCanvas);

  std::list<QPoint> getPoints() const { return myPoints; }
  void              setPoints(std::list<QPoint> thePoints) { myPoints = thePoints; }
  void              updatePoints(QCanvasItem* thePointItem);

  bool isEmptyPrs() const { return myPrs.isEmpty(); }
  
  virtual void setHilighted(bool state);
  virtual void setSelected(bool state);
  void setColor(const QColor& theColor);
  
  virtual void moveByPort(YACSPrs_Port* thePort, bool theMoveInternalLinkPoints=false, QRect theArea=QRect()) {}
  virtual void moveByPort(YACSPrs_Port* thePort, int dx, int dy) {}
  
  void setSelectedObject(QCanvasItem* theItem, const QPoint& thePoint);
    
  virtual QString getToolTipText() const;
  
  bool isFirst(QCanvasEllipse* thePoint);
  bool isLast(QCanvasEllipse* thePoint);

  QCanvasItem* getFirstPoint() const { return myPrs.first(); }
  QCanvasItem* getLastPoint() const { return myPrs.last(); }

  void setMyZ(double z);
  double getMyZ();

  void setZ(double z);
  double z();

 public slots:
  virtual void remove();
 
  void addPoint();
  void removePoint();

  QPoint getConnectionPoint(YACSPrs_Port* thePort);

 protected:
  void addPoint(const QPoint& thePoint, const int& theIndex = -1);
  virtual void createPrs();

  SUIT_ResourceMgr* myMgr;
  QCanvas*          myCanvas;
  QCanvasItemList   myPrs;

  std::list<QPoint> myPoints; //for read/write from/to xml
  
  QColor myColor;
  
 private:
  bool myHilighted;
  bool mySelected;

  QCanvasItem* mySelectedItem;
  QPoint       mySelectedPoint;
  
  double myZ;
};

class YACSPrs_PortLink : public YACSPrs_Link, public YACS::HMI::GuiObserver {

 public:
  YACSPrs_PortLink(SUIT_ResourceMgr* theMgr, QCanvas* theCanvas, YACSPrs_InOutPort* theInputPort, YACSPrs_InOutPort* theOutputPort);
  virtual ~YACSPrs_PortLink();

  virtual void select( bool isSelected );
  virtual void update( YACS::HMI::GuiEvent event, int type, YACS::HMI::Subject* son);

  YACS::HMI::Subject* getSubject() const;

  virtual void moveByPort(YACSPrs_Port* thePort, bool theMoveInternalLinkPoints=false, QRect theArea=QRect());
  virtual void moveByPort(YACSPrs_Port* thePort, int dx, int dy);

  virtual QString getToolTipText() const;

  virtual void remove();

  YACSPrs_InOutPort* getInputPort() const { return myInputPort; }
  void               setInputPort(YACSPrs_InOutPort* thePort) { myInputPort = thePort; }

  YACSPrs_InOutPort* getOutputPort() const { return myOutputPort; }
  void               setOutputPort(YACSPrs_InOutPort* thePort) { myOutputPort = thePort; }

  virtual void setHilighted(bool state);
  virtual void setSelected(bool state);

 protected:
  virtual void createPrs();
  
 private:
  YACSPrs_InOutPort* myInputPort;
  YACSPrs_InOutPort* myOutputPort;
};

class YACSPrs_LabelLink : public YACSPrs_Link {

 public:
  YACSPrs_LabelLink(SUIT_ResourceMgr* theMgr, QCanvas* theCanvas, YACSPrs_LabelPort* theOutputPort, YACSPrs_ElementaryNode* theSlaveNode);
  virtual ~YACSPrs_LabelLink();

  virtual void moveByPort(YACSPrs_Port* thePort, bool theMoveInternalLinkPoints=false, QRect theArea=QRect());
  virtual void moveByPort(YACSPrs_Port* thePort, int dx, int dy);

  void moveByNode(YACSPrs_ElementaryNode* theNode);
  void moveByNode(YACSPrs_ElementaryNode* theNode, int dx, int dy);

  virtual QString getToolTipText() const;

  virtual void remove();

  YACSPrs_LabelPort*      getOutputPort() const { return myOutputPort; }
  void                    setOutputPort(YACSPrs_LabelPort* thePort) { myOutputPort = thePort; }

  YACSPrs_ElementaryNode* getSlaveNode() const { return mySlaveNode; }
  void                    setSlaveNode(YACSPrs_ElementaryNode* theNode) { mySlaveNode = theNode; }

  virtual void setHilighted(bool state);
  virtual void setSelected(bool state);
  
  QPoint getConnectionMasterPoint();

 protected:
  virtual void createPrs();
  
 private:
  YACSPrs_LabelPort*      myOutputPort;
  YACSPrs_ElementaryNode* mySlaveNode;
};

class YACSPrs_Edge;

/*! Link point presentation.
 */
class YACSPrs_Point : public QxGraph_ActiveItem, public QCanvasEllipse {

  public:
    YACSPrs_Point(QCanvas* theCanvas, YACSPrs_Link* theLink, const int& theIndex = -1);
    ~YACSPrs_Point() {}

    /* reimplement functions from QxGraph_ActiveItem */
    virtual bool isMoveable();
    virtual void beforeMoving() {}
    virtual void afterMoving() {}
    virtual void hilight(const QPoint& theMousePos, const bool toHilight = true);
    virtual void select(const QPoint& theMousePos, const bool toSelect = true);
    virtual void showPopup(QWidget* theParent, QMouseEvent* theEvent, const QPoint& theMousePos = QPoint());
    virtual QString getToolTipText(const QPoint& theMousePos, QRect& theRect) const;
    virtual bool arePartsOfOtherItem(QxGraph_ActiveItem* theSecondItem);
    
    YACSPrs_Link* getLink() const { return myLink; }
    void setIndex(int theIndex) { myIndex = theIndex; }
    int getIndex() const { return myIndex; }

    void setInEdge(YACSPrs_Edge* theEdge);
    void setOutEdge(YACSPrs_Edge* theEdge);

    void moveBy(double dx, double dy);
    void setColor(const QColor& theColor);

    void setMoving(bool b) { myMoving = b; }
    bool isMoving() const { return myMoving; }

    void setSelected(bool b) { mySelected = b; }

    virtual int rtti() const;

  private:
    YACSPrs_Link* myLink;
    int myIndex;
    bool myMoving;

    YACSPrs_Edge* myInEdge;
    YACSPrs_Edge* myOutEdge;

    bool mySelected;
};

/*! Link edge presentation.
 */
class YACSPrs_Edge : public QxGraph_ActiveItem, public QCanvasLine {

  public:
    YACSPrs_Edge(QCanvas* theCanvas, YACSPrs_Link* theLink);
    ~YACSPrs_Edge();

    /* reimplement functions from QxGraph_ActiveItem */
    virtual bool isMoveable();
    virtual void beforeMoving() {}
    virtual void afterMoving() {}
    virtual void hilight(const QPoint& theMousePos, const bool toHilight = true);
    virtual void select(const QPoint& theMousePos, const bool toSelect = true);
    virtual void showPopup(QWidget* theParent, QMouseEvent* theEvent, const QPoint& theMousePos = QPoint());
    virtual QString getToolTipText(const QPoint& theMousePos, QRect& theRect) const;
    virtual bool arePartsOfOtherItem(QxGraph_ActiveItem* theSecondItem);
    void setCanvas(QCanvas* theCanvas);

    YACSPrs_Link* getLink() const { return myLink; }

    void setFromPoint(YACSPrs_Point* thePoint);
    void setToPoint(YACSPrs_Point* thePoint);

    void moveBy(double dx, double dy);
    void setColor(const QColor& theColor);

    void setMoving(bool b) { myMoving = b; }
    bool isMoving() const { return myMoving; }

    void setSelected(bool b) { mySelected = b; }
    virtual void setArrow();
    virtual void setVisible ( bool ) ;
    virtual void setZ ( double z );

    virtual int rtti() const;

  private:
    bool myMoving;
    YACSPrs_Link* myLink;

    // for moving segment of link
    YACSPrs_Point* myStartPoint;
    YACSPrs_Point* myEndPoint;

    bool mySelected;
    QCanvasPolygon* myArrow;
};

#endif
