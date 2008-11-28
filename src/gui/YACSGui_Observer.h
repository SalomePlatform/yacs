//  Copyright (C) 2006-2008  CEA/DEN, EDF R&D, OPEN CASCADE
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
#ifndef YACSGui_Observer_HeaderFile
#define YACSGui_Observer_HeaderFile

#include <yacsgui.hh>
#include <Proc.hxx>
#include <Dispatcher.hxx>
#include <qobject.h>
#include <qsemaphore.h>
#include <qevent.h>
#include <list>
#include <map>
#include <string>

class YACSGui_Graph;
class YACSGui_Module;
class YACSGui_Executor;
class YACS::ENGINE::Node;

#define YACS_EVENT QEvent::Type( QEvent::User + 7983 )

class YACSEvent : public QCustomEvent
{
 public:
  YACSEvent(std::pair<int,std::string> aYACSEvent);
  std::pair<int,std::string> getYACSEvent() const { return _event; };
 private:
    std::pair<int,std::string> _event;
};


class YACSGui_Observer : public YACS::ENGINE::Observer
{
 public:
  YACSGui_Observer(YACSGui_Graph* theGraph);
  
  virtual void notifyObserver(YACS::ENGINE::Node* object,const std::string& event);
  
  void notifyObserver(const int theID, const std::string& theEvent);
  void notifyObserver(const std::string& theName, const std::string& theEvent);

  void setNodeState(const int theID, const int theState);
  void setNodeState(const std::string& theName, const int theState);

 private:
  YACSGui_Graph* myGraph;
};


class Observer_i : public POA_YACS_ORB::Observer,
  public PortableServer::RefCountServantBase,
  public QObject
{
 public:
  Observer_i(YACS::ENGINE::Proc* guiProc,
             YACSGui_Module* guiMod,
             YACSGui_Executor* guiExec);
  virtual ~Observer_i();
  void notifyObserver(CORBA::Long numid, const char* event);
  void setConversion();
  void SetImpl(YACSGui_Observer* theImpl);
  void SetRemoteProc(YACS_ORB::ProcExec_ptr engineProc);
  int getEngineId(int guiId) { return _guiToEngineMap[guiId]; };
 protected:
  virtual bool event(QEvent *e);
 private:
  YACSGui_Module* _guiMod;
  YACSGui_Executor* _guiExec;
  YACS::ENGINE::Proc* _guiProc;
  YACS_ORB::ProcExec_var _engineProc;
  YACSGui_Observer* myImpl;
  std::map<int, int> _guiToEngineMap;
  std::map<int, int> _engineToGuiMap;
};


#endif
