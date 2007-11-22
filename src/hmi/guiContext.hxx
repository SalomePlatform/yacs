
#ifndef _GUICONTEXT_HXX_
#define _GUICONTEXT_HXX_

#include "Proc.hxx"
#include "Catalog.hxx"
#include "commandsProc.hxx"
#include "guiObservers.hxx"

#include <qcanvas.h>
#include <qwidgetstack.h>
#include <map>
#include <string>

namespace YACS
{
  namespace HMI
  {
//     class Subject;

    class GuiContext: public Subject
    {
    public:
      GuiContext();
      virtual ~GuiContext();
      void setProc(YACS::ENGINE::Proc* proc);

      inline YACS::ENGINE::Catalog* getBuiltinCatalog()  {return _builtinCatalog; };
      inline YACS::ENGINE::Catalog* getSessionCatalog()  {return _sessionCatalog; };
      inline YACS::ENGINE::Proc* getProc()               {return _proc; };
      inline YACS::HMI::ProcInvoc* getInvoc()            {return _invoc; };
      inline YACS::HMI::SubjectProc* getSubjectProc()    {return _subjectProc; };
      inline QCanvas* getCanvas()                        {return _canvas; };
      inline QCanvasItem* getSelectedCanvasItem()        {return _selectedCanvasItem; };
      inline long getNewId()                             {return _numItem++; };
      inline QWidgetStack* getWidgetStack()              {return _widgetStack; };

      inline void setSelectedCanvasItem(QCanvasItem *c)          {_selectedCanvasItem = c; };
      inline void setWidgetStack(QWidgetStack* ws)               {_widgetStack = ws; };
      inline void setSessionCatalog(YACS::ENGINE::Catalog* cata) {_sessionCatalog = cata; };

      inline static GuiContext* getCurrent()             {return _current; };
      inline static void setCurrent(GuiContext* context) { _current=context; };

      std::map<YACS::ENGINE::Node*,YACS::HMI::SubjectNode*> _mapOfSubjectNode;
      std::map<YACS::ENGINE::DataPort*,YACS::HMI::SubjectDataPort*> _mapOfSubjectDataPort;
      std::map<std::pair<YACS::ENGINE::OutPort*, YACS::ENGINE::InPort*>,YACS::HMI::SubjectLink*> _mapOfSubjectLink;
      std::map<YACS::ENGINE::ComponentInstance*, YACS::HMI::SubjectComponent*> _mapOfSubjectComponent;
      std::map<YACS::ENGINE::Container*, YACS::HMI::SubjectContainer*> _mapOfSubjectContainer;

    protected:
      YACS::ENGINE::Catalog* _builtinCatalog;
      YACS::ENGINE::Catalog* _sessionCatalog;
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
