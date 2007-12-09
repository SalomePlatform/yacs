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
#ifndef YACSPRS_ELEMENTARYNODE_H
#define YACSPRS_ELEMENTARYNODE_H

#include <Port.hxx>
#include <Node.hxx>
#include <guiObservers.hxx>

#include "QxGraph_ActiveItem.h"

#include <qcanvas.h>
#include <qdatetime.h>


class SUIT_ResourceMgr;

class YACSPrs_Port;
class YACSPrs_ElementaryNode;

class YACSPrs_Hook : public QxGraph_ActiveItem, public QCanvasEllipse
{
  public:
    YACSPrs_Hook(SUIT_ResourceMgr* theMgr, QCanvas* theCanvas, YACSPrs_ElementaryNode* theNode, 
		 const bool& theIn, const bool& theGate, const bool& theMaster=false);
    YACSPrs_Hook(SUIT_ResourceMgr* theMgr, QCanvas* theCanvas, YACSPrs_Port* thePort, 
		 const bool& theIn, const bool& theGate, const bool& theMaster=false);
    ~YACSPrs_Hook();

    virtual void setCanvas(QCanvas* theCanvas);

    /* reimplement functions from QxGraph_ActiveItem */
    virtual bool isMoveable() { return false; }
    virtual void beforeMoving() {}
    virtual void afterMoving() {}
    virtual void hilight(const QPoint& theMousePos, const bool toHilight = true);
    virtual void select(const QPoint& theMousePos, const bool toSelect = true);
    virtual void showPopup(QWidget* theParent, QMouseEvent* theEvent, const QPoint& theMousePos = QPoint());
    virtual QString getToolTipText(const QPoint& theMousePos, QRect& theRect) const { return QString(""); }
    
    QObject* getObject() const;

    void setCoords(int x, int y);

    void setColor(const QColor& theColor);
    
    void setVisible(bool b);
    void moveBy(double dx, double dy);
    void setZ(double z);

    void setSelected(bool b) { mySelected = b; }

    virtual int rtti() const;

    QPointArray areaPoints() const;

  protected:
    //void draw(QPainter& thePainter);
    virtual void drawShape(QPainter& thePainter);

    SUIT_ResourceMgr* myMgr;

  private:
    void init(QCanvas* theCanvas);

    YACSPrs_ElementaryNode* myNodePrs;
    YACSPrs_Port*           myPortPrs;

    bool myIn;
    bool myGate;
    bool myMaster;

    QCanvasLine* myLine;
    QPoint       myTrPoint;

    bool mySelected;
};

class YACSPrs_Link;
class YACSPrs_ElementaryNode;
class YACSPrs_Port {

 public:
  YACSPrs_Port( SUIT_ResourceMgr*, QCanvas*, YACSPrs_ElementaryNode*);
  ~YACSPrs_Port();

  std::list<YACSPrs_Link*> getLinks() const { return myLinks; }
  YACSPrs_ElementaryNode* getNode() const { return myNode; }

  void setCanvas(QCanvas* theCanvas);

  void addLink(YACSPrs_Link* theLink) { myLinks.push_back(theLink); }
  void removeLink(YACSPrs_Link* theLink) { if (!myLinks.empty()) myLinks.remove(theLink); }
  void updateLinks(bool theMoveInternalLinkPoints=false, QRect theArea=QRect());
  YACSPrs_Hook* GetHook() const { return myPoint; }

  virtual QString getName() const;

  virtual void setPortRect(const QRect& theRect, bool theMoveInternalLinkPoints=false, QRect theArea=QRect()) = 0;
  virtual QRect getPortRect() const = 0;

  void setVisible(bool b);
  void changeVisibility(bool b) { myVisible = b; }
  bool isVisible() const { return myVisible; }

  virtual void setColor(const QColor& theColor, bool theUpdatePointColor=true, 
			bool theUpdate=false, bool theSelectionProcess=false);
  QColor Color() const { return myColor; }

  virtual void setStoreColor(const QColor& theColor) { myStoreColor = theColor; }
  QColor storeColor() const { return myStoreColor; }

  virtual void moveBy(int dx, int dy);
  virtual void setZ(double z, bool updateLinksZ);
  double z();

  void setSelected(bool b) { mySelected = b; }
  bool isSelected() const { return mySelected; }

  virtual QPoint getConnectionPoint() const = 0;
  virtual void draw(QPainter& thePainter, int theXRnd, int theYRnd) = 0;

 protected:
  SUIT_ResourceMgr*     myMgr;

  QCanvas*              myCanvas;

  QRect                 myNameRect;
  QString               myName;

  YACSPrs_Hook*         myPoint;
  bool                  myVisible;

  QColor                myColor;
  QColor                myStoreColor;

  std::list<YACSPrs_Link*> myLinks;

 private:
  bool                    mySelected;

  YACSPrs_ElementaryNode* myNode;
};

class YACSPrs_LabelPort : public YACSPrs_Port {

 public:
  YACSPrs_LabelPort( SUIT_ResourceMgr*, QCanvas*, YACS::ENGINE::Node*, YACSPrs_ElementaryNode*,
		     const bool& theSwitch=false, const int& theId=-1);
  ~YACSPrs_LabelPort();

  YACS::ENGINE::Node* getSlaveNode() const { return mySlaveNode; }
  void setSlaveNode( YACS::ENGINE::Node* theNode ) { mySlaveNode = theNode; }
  
  void setName(QString theName) { myName = theName; }

  virtual void setPortRect(const QRect& theRect, bool theMoveInternalLinkPoints=false, QRect theArea=QRect());
  virtual QRect getPortRect() const;

  virtual void setColor(const QColor& theColor, bool theUpdatePointColor=true,
			bool theUpdate=false, bool theSelectionProcess=false);

  virtual void moveBy(int dx, int dy);

  virtual QPoint getConnectionPoint() const;
  virtual void draw(QPainter& thePainter, int theXRnd, int theYRnd);

 private:
  YACS::ENGINE::Node*   mySlaveNode; // engine of slave node (i.e. the node on this case for Switch or
                                     //                       the node inside the loop body for Loop)
};

class YACSPrs_InOutPort : public YACSPrs_Port {

 public:
  YACSPrs_InOutPort( SUIT_ResourceMgr*, QCanvas*, YACS::ENGINE::Port*, YACSPrs_ElementaryNode* );
  ~YACSPrs_InOutPort();

  YACS::ENGINE::Port* getEngine() const { return myEngine; }

  bool isInput() const { return myInput; }
  bool isGate() const { return myGate; }

  virtual void update(bool theForce = false, YACS::ENGINE::Port* theEngine = 0);

  virtual bool isHilight() const;
  virtual bool isAlert() const;

  virtual QString getName() const;
  virtual QString getType(const bool forToolTip = false) const;
  virtual QString getValue(YACS::ENGINE::Port* theEngine = 0) const;

  virtual int getAlignment() const;

  virtual void setPortRect(const QRect& theRect, bool theMoveInternalLinkPoints=false, QRect theArea=QRect());
  virtual QRect getPortRect() const;

  virtual void moveBy(int dx, int dy);
  virtual void setZ(double z, bool updateLinksZ);

  virtual QPoint getConnectionPoint() const;
  virtual void draw(QPainter& thePainter, int theXRnd, int theYRnd);

 private:
  YACS::ENGINE::Port*   myEngine; // port engine

  QRect                 myTypeRect;
  QRect                 myValueRect;

  QString               myType;
  QString               myValue;

  bool                  myInput; // to remove when connect with engine
  bool                  myGate; // to remove when connect with engine
};

class YACSPrs_LabelLink;
class YACSPrs_ElementaryNode : public QxGraph_ActiveItem,
			       public QCanvasPolygonalItem,
			       public YACS::HMI::GuiObserver {
 public:
  YACSPrs_ElementaryNode( SUIT_ResourceMgr*, QCanvas*, YACS::HMI::SubjectNode* );
  virtual ~YACSPrs_ElementaryNode();

   virtual void select( bool isSelected );
   virtual void update( YACS::HMI::GuiEvent event, int type, YACS::HMI::Subject* son);

  YACS::ENGINE::Node* getEngine() const;
  YACS::HMI::SubjectNode* getSEngine() const { return mySEngine; }

  QPtrList<YACSPrs_Port> getPortList() const { return myPortList; }
  
  YACSPrs_InOutPort* getPortPrs(YACS::ENGINE::Port* thePort);

  void sortPorts();
  void moveUp(YACS::ENGINE::Port* thePort);
  void moveDown(YACS::ENGINE::Port* thePort);
  void addPortPrs(YACS::ENGINE::Port* thePort);
  void removePortPrs(YACS::ENGINE::Port* thePort);

  virtual void setCanvas(QCanvas* theCanvas);

  void addLabelLink(YACSPrs_LabelLink* theLink) { myLabelLink = theLink; }
  void removeLabelLink();
  void updateLabelLink();
  YACSPrs_LabelLink* getLabelLink() const { return myLabelLink; }

  /* reimplement functions from QxGraph_ActiveItem */
  virtual bool isMoveable() { return true; }
  virtual void beforeMoving();
  virtual void afterMoving();
  virtual void hilight(const QPoint& theMousePos, const bool toHilight = true);
  virtual void select(const QPoint& theMousePos, const bool toSelect = true);
  virtual void showPopup(QWidget* theParent, QMouseEvent* theEvent, const QPoint& theMousePos = QPoint());
  virtual QString getToolTipText(const QPoint& theMousePos, QRect& theRect) const;

  void setMoving(bool b) { myMoving = b; }
  bool isMoving() const { return myMoving; }

  void setSelfMoving(bool b) { mySelfMoving = b; }
  bool isSelfMoving() const { return mySelfMoving; }

  void setSelected(bool b) { mySelected = b; }
  bool isSelected() const { return mySelected; }

  virtual int rtti() const;

  virtual void setVisible(bool b);

  QPointArray areaPoints() const;
  virtual QPointArray maxAreaPoints() const;
  virtual QPointArray constructAreaPoints(int theW, int theH) const;

  void moveBy(double dx, double dy);
  void setZ(double z);
  void setZFromLinks(double z);

  virtual void update();
  virtual void updateForEachLoopBody(YACS::ENGINE::Node* theEngine=0);
  virtual void updatePorts(bool theForce=false);
  virtual void updateGates(bool theCreate);

  virtual void setNodeColor(const QColor& theColor);
  virtual QColor nodeColor() const { return myColor; }

  virtual void setNodeSubColor(const QColor& theColor, bool theSelectionProcess=false);
  virtual QColor nodeSubColor() const { return mySubColor; }

  void setStoreColor(const QColor& theColor) { myStoreColor = theColor; }
  QColor storeColor() const { return myStoreColor; }

  void setStoreSubColor(const QColor& theColor) { myStoreSubColor = theColor; }
  QColor storeSubColor() const { return myStoreSubColor; }

  virtual int width() const { return myWidth; }
  virtual int height() const;

  virtual int maxWidth() const;
  virtual int maxHeight() const;

  virtual int getTitleHeight() const { return myTitleHeight; }
  void setTitleHeight(int theTitleHeight) { myTitleHeight = theTitleHeight; }

  virtual int getInfoHeight() const;
  virtual int getStatusHeight() const;
  virtual int getTimeHeight() const;
  virtual int getBodyHeight() const;
  virtual int getGateHeight() const;
  
  virtual int getPixMapHeight() const;
  virtual int getPixMapWidth() const;

  virtual QRect getRect() const;
  virtual QRect getTitleRect() const;
  virtual QRect getPixmapRect(bool theLeft=false, bool theService=false) const;
  virtual QRect getBodyRect() const;
  virtual QRect getGateRect() const;

  virtual QRect getWholeRect() const { return QRect(); }   // a whole progress bar area
  virtual QRect getPercentRect() const { return QRect(); } // filled area of the progress bar

  void setTimeIteration(double theTimeIteration) { myTimeIteration = theTimeIteration; }
  double getTimeIteration() const { return myTimeIteration; }

  double getStoredPercentage() const { return myPercentage; }

  virtual void nextTimeIteration(YACS::ENGINE::Node* theEngine=0);
  virtual int  getPercentage(YACS::ENGINE::Node* theEngine=0) const;
  
  void    updateExecTime(YACS::ENGINE::Node* theEngine=0);
  bool    isFinished() const { return myFinished; }

  virtual void setState(YACS::StatesForNode theState); // set state and update pixmap
                     /*state type from engine (may be enum)*/

  YACSPrs_Hook* getMasterPoint() const { return myPointMaster; }
  virtual QPoint getConnectionMasterPoint();
  void setMasterPointColor(QColor theColor);

  // for constraint nodes' moving inside the Bloc-->
  bool isInBloc() { return myIsInBloc; }
  void setIsInBloc(bool theIsInBloc) { myIsInBloc = theIsInBloc; }
  void setArea(QRect theArea) { myArea = theArea; }
  virtual bool checkArea(double dx, double dy);
  // <--

  bool isCheckAreaNeeded() const { return myIsCheckAreaNeeded; }
  void setIsCheckAreaNeeded(bool theValue) { myIsCheckAreaNeeded = theValue; }

 //signals:
  //void portsChanged();

 protected:
  void draw(QPainter& thePainter);
  virtual void drawShape(QPainter& thePainter);
  virtual void drawTitleShape(QPainter& thePainter);
  
  virtual void drawTitle(QPainter& thePainter);
  virtual void drawPort(QPainter& thePainter);
  virtual void drawGate(QPainter& thePainter);

  virtual void drawFrame(QPainter& thePainter);
  void drawBoundary(QPainter& thePainter, int theRightBottomPointIndex);

  SUIT_ResourceMgr*   myMgr;

  YACS::HMI::SubjectNode* mySEngine; // node subject

  QPixmap myStatePixmap;
  //QCanvasSprite* mySprite; // for states animation

  QString myStatus;
  QString myTime;

  int myWidth;

  int myXRnd;
  int myYRnd;

  int myPortHeight;

  QPtrList<YACSPrs_Port> myPortList;
  YACSPrs_Port*          myHilightedPort;
  YACSPrs_Port*          mySelectedPort;

  double myZ;

  YACSPrs_Hook*      myPointMaster;
  YACSPrs_LabelLink* myLabelLink;

  // for constraint nodes' moving inside the Bloc-->
  bool  myIsInBloc;
  QRect myArea;
  // <--
  
 private:
  int myTitleHeight;
  int myStatusHeight;
  int myTimeHeight;
  int myGateHeight;

  QColor myColor;
  QColor mySubColor;
  QColor myStatusColor;

  QColor myStoreColor;
  QColor myStoreSubColor;
  
  YACSPrs_Hook* myPointIn;
  YACSPrs_Hook* myPointOut;

  bool myMoving;
  bool mySelfMoving; // field to indicate if node is moving itself
                     // or as a content of moving Bloc node
  bool mySelected;

  // for update node geometry after import from XML file
  bool myIsCheckAreaNeeded;

  // for progress bar indicator
  double myTimeIteration;
  double myPercentage; // note : use for update original body node of ForEachLoop node only

  // for execution time measuring
  QTime myStartTime;
  bool  myStarted;
  bool  myFinished;
};

#endif
