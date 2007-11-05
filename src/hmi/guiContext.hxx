
#ifndef _GUICONTEXT_HXX_
#define _GUICONTEXT_HXX_

#include "Proc.hxx"
#include "Catalog.hxx"
#include "commandsProc.hxx"
#include "guiObservers.hxx"

#include <qcanvas.h>
#include <qwidgetstack.h>
#include <map>

namespace YACS
{
  namespace HMI
  {
//     class Subject;

    class GuiContext: public Subject
    {
    public:
      GuiContext(YACS::ENGINE::Proc* proc);
      virtual ~GuiContext();
      inline YACS::ENGINE::Catalog* getBuiltinCatalog()  {return _builtinCatalog; };
      inline YACS::ENGINE::Proc* getProc()               {return _proc; };
      inline YACS::HMI::ProcInvoc* getInvoc()            {return _invoc; };
      inline YACS::HMI::SubjectProc* getSubjectProc()    {return _subjectProc; };
      inline QCanvas* getCanvas()                        {return _canvas; };
      inline QCanvasItem* getSelectedCanvasItem()        {return _selectedCanvasItem; };
      inline void setSelectedCanvasItem(QCanvasItem *c)  {_selectedCanvasItem = c; };
      inline long getNewId()                             {return _numItem++; };
      inline QWidgetStack* getWidgetStack()              {return _widgetStack; };
      inline void setWidgetStack(QWidgetStack* ws)       {_widgetStack = ws; };
      inline static GuiContext* getCurrent()             {return _current; };
      inline static void setCurrent(GuiContext* context) { _current=context; };

    protected:
      YACS::ENGINE::Catalog* _builtinCatalog;
      YACS::ENGINE::Proc* _proc;
      YACS::HMI::ProcInvoc* _invoc;
      YACS::HMI::SubjectProc *_subjectProc;
      long _numItem;
      static GuiContext* _current;
      QCanvas *_canvas;
      QCanvasItem *_selectedCanvasItem;
      QWidgetStack* _widgetStack;
    };
  }
}
#endif
