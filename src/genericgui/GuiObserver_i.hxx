
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
