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
#ifndef _EDITCANVAS_H_
#define _EDITCANVAS_H_

#include "guiObservers.hxx"

#include <qcanvas.h>

#include <string>

namespace YACS
{
  namespace HMI
  {
    class CanvasItem;
    class ComposedNodeCanvasItem;

    class EditCanvas : public QCanvasView, public GuiObserver
    {
      Q_OBJECT
        
        public:
      EditCanvas(YACS::HMI::Subject *context,
                 QCanvas* c, QWidget* parent=0, const char* name=0, WFlags f=0);
      virtual void update(GuiEvent event, int type, Subject* son);
      void setNewRoot(YACS::HMI::Subject *root);
      
    protected:
      YACS::HMI::Subject *_context;
      ComposedNodeCanvasItem *_rootItem;
      void contentsMousePressEvent(QMouseEvent* e);
      
    signals:
      
    private:
    };

    class CanvasItem : public QCanvasRectangle, public GuiObserver
    {
    public:
      CanvasItem(CanvasItem *parent, QString label, Subject* subject);
      virtual ~CanvasItem();
      CanvasItem *getParent();
      CanvasItem *getRoot();
      virtual void select(bool isSelected);
      virtual QColor getNormalColor();
      virtual QColor getSelectedColor();
      virtual Subject* getSubject();
      int getLevel();
      virtual int getOfx();
      virtual int getOfy();
      virtual int getLx();
      virtual int getLy();
      virtual int getOfxInPort(CanvasItem *child);
      virtual int getOfxOutPort(CanvasItem *child);
      virtual void redraw();
      virtual void drawNode(int ofx, int ofy, int lx, int ly, int level);
    protected:
      Subject* _subject;
      CanvasItem *_parent;
      CanvasItem *_root;
      int _level;
      int _ofx, _ofy, _lx, _ly;
      QString _label;
    };

    class NodeCanvasItem: public CanvasItem
    {
    public:
      NodeCanvasItem(CanvasItem *parent, QString label, Subject* subject);
      virtual ~NodeCanvasItem();
      virtual void removeInPort(CanvasItem* inport);
      virtual void removeOutPort(CanvasItem* outport);
      std::list<CanvasItem*> _inPorts;
      std::list<CanvasItem*> _outPorts;      
    };

    class ElementaryNodeCanvasItem: public NodeCanvasItem
    {
    public:
      ElementaryNodeCanvasItem(CanvasItem *parent, QString label, Subject* subject);
      virtual ~ElementaryNodeCanvasItem();
      virtual QColor getNormalColor();
      virtual QColor getSelectedColor();
      virtual void update(GuiEvent event, int type, Subject* son);
      virtual int getLx();
      virtual int getLy();
      virtual void redraw();
      virtual int getOfxInPort(CanvasItem *child);
      virtual int getOfxOutPort(CanvasItem *child);
    };

    class ComposedNodeCanvasItem: public NodeCanvasItem
    {
    public:
      ComposedNodeCanvasItem(CanvasItem *parent, QString label, Subject* subject);
      virtual ~ComposedNodeCanvasItem();
      virtual QColor getNormalColor();
      virtual QColor getSelectedColor();
      virtual void update(GuiEvent event, int type, Subject* son);
      virtual void removeChild(CanvasItem* child);
      virtual int getLx();
      virtual int getLy();
      virtual int getOfyChild(CanvasItem *child);
      virtual void redraw();
      virtual int getOfxInPort(CanvasItem *child);
      virtual int getOfxOutPort(CanvasItem *child);
    protected:
      std::list<CanvasItem*> _children;
    };

    class InPortCanvasItem: public CanvasItem
    {
    public:
      InPortCanvasItem(CanvasItem *parent, QString label, Subject* subject);
      virtual ~InPortCanvasItem();
      virtual QColor getNormalColor();
      virtual QColor getSelectedColor();
      virtual void update(GuiEvent event, int type, Subject* son);
      virtual int getLx();
      virtual int getLy();
      virtual int getOfx();
      virtual int getOfy();
      virtual void redraw();
    };

    class OutPortCanvasItem: public CanvasItem
    {
    public:
      OutPortCanvasItem(CanvasItem *parent, QString label, Subject* subject);
      virtual ~OutPortCanvasItem();
      virtual QColor getNormalColor();
      virtual QColor getSelectedColor();
      virtual void update(GuiEvent event, int type, Subject* son);
      virtual int getLx();
      virtual int getLy();
      virtual int getOfx();
      virtual int getOfy();
      virtual void redraw();
    };

  }
}

#endif

