// Copyright (C) 2006-2016  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
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

#ifndef _GUIOBSERVER_I_HXX_
#define _GUIOBSERVER_I_HXX_

#include <yacsgui.hh>

#include <QEvent>

#include <map>
#include <string>

namespace YACS
{
  namespace ENGINE
  {
    class Proc;
  }

  namespace HMI
  {
    class GuiExecutor;

    class YACSEvent : public QEvent
    {
    public:
      YACSEvent(std::pair<int,std::string> aYACSEvent);
      std::pair<int,std::string> getYACSEvent() const { return _event; };
    private:
      std::pair<int,std::string> _event;
    };


    class GuiObserver_i : public POA_YACS_ORB::Observer,
                          public PortableServer::RefCountServantBase
    {
      friend class GuiExecutor;
    public:
      GuiObserver_i(YACS::ENGINE::Proc* guiProc);
      virtual ~GuiObserver_i();
      void notifyObserver(CORBA::Long numid, const char* event);
      void setConversion();
      void SetImpl(GuiExecutor* impl);
      void SetRemoteProc(YACS_ORB::ProcExec_ptr engineProc);
      int getEngineId(int guiId) { return _guiToEngineMap[guiId]; };

    protected:
      YACS::ENGINE::Proc* _guiProc;
      YACS_ORB::ProcExec_var _engineProc;
      GuiExecutor* _impl;
      std::map<int, int> _guiToEngineMap;
      std::map<int, int> _engineToGuiMap;
    };
  }
}

#endif
