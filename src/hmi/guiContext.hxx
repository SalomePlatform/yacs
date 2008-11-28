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
      long getNewId(YACS::HMI::TypeOfElem type);

      inline YACS::ENGINE::Catalog* getBuiltinCatalog()        {return _builtinCatalog; };
      inline YACS::ENGINE::Catalog* getSessionCatalog()        {return _sessionCatalog; };
      inline YACS::ENGINE::Catalog* getProcCatalog()           {return _procCatalog; };
      inline YACS::ENGINE::Catalog* getCurrentCatalog()        {return _currentCatalog; };

      inline YACS::ENGINE::Proc* getProc()                     {return _proc; };
      inline YACS::HMI::ProcInvoc* getInvoc()                  {return _invoc; };
      inline YACS::HMI::SubjectProc* getSubjectProc()          {return _subjectProc; };
      inline QCanvas* getCanvas()                              {return _canvas; };
      inline QCanvasItem* getSelectedCanvasItem()              {return _selectedCanvasItem; };
      inline long getNewId()                                   {return _numItem++; };
      inline QWidgetStack* getWidgetStack()                    {return _widgetStack; };
      inline std::string getXMLSchema()                        {return _xmlSchema; };
      inline std::pair<std::string, std::string> getYACSCont() {return _YACSEngineContainer; }

      inline void setSelectedCanvasItem(QCanvasItem *c)                    {_selectedCanvasItem = c; };
      inline void setWidgetStack(QWidgetStack* ws)                         {_widgetStack = ws; };
      inline void setSessionCatalog(YACS::ENGINE::Catalog* cata)           {_sessionCatalog = cata; };
      inline void setProcCatalog(YACS::ENGINE::Catalog* cata)              {_procCatalog = cata; };
      inline void setCurrentCatalog(YACS::ENGINE::Catalog* cata)           {_currentCatalog = cata; };
      inline void setXMLSchema(std::string xmlSchema)                      {_xmlSchema = xmlSchema; };
      inline void setYACSContainer(std::pair<std::string, std::string> yc) {_YACSEngineContainer = yc; };

      inline static GuiContext* getCurrent()             {return _current; };
      inline static void setCurrent(GuiContext* context) { _current=context; };

      std::map<YACS::ENGINE::Node*,YACS::HMI::SubjectNode*>                                        _mapOfSubjectNode;
      std::map<YACS::ENGINE::DataPort*,YACS::HMI::SubjectDataPort*>                                _mapOfSubjectDataPort;
      std::map<std::pair<YACS::ENGINE::OutPort*, YACS::ENGINE::InPort*>,YACS::HMI::SubjectLink*>   _mapOfSubjectLink;
      std::map<std::pair<YACS::ENGINE::Node*, YACS::ENGINE::Node*>,YACS::HMI::SubjectControlLink*> _mapOfSubjectControlLink;
      std::map<YACS::ENGINE::ComponentInstance*, YACS::HMI::SubjectComponent*>                     _mapOfSubjectComponent;
      std::map<YACS::ENGINE::Container*, YACS::HMI::SubjectContainer*>                             _mapOfSubjectContainer;
      std::map<std::string, YACS::HMI::SubjectDataType*>                                           _mapOfSubjectDataType;
      std::string _lastErrorMessage;

    protected:
      YACS::ENGINE::Catalog* _builtinCatalog;
      YACS::ENGINE::Catalog* _sessionCatalog;
      YACS::ENGINE::Catalog* _procCatalog;
      YACS::ENGINE::Catalog* _currentCatalog;
      YACS::ENGINE::Proc* _proc;
      YACS::HMI::ProcInvoc* _invoc;
      YACS::HMI::SubjectProc *_subjectProc;
      long _numItem;
      static GuiContext* _current;
      QCanvas *_canvas;
      QCanvasItem *_selectedCanvasItem;
      QWidgetStack* _widgetStack;
      std::string _xmlSchema;
      std::pair<std::string, std::string> _YACSEngineContainer; // --- <ContainerName, HostName>

    };
  }
}
#endif
