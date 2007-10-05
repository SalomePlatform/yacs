
#ifndef __SALOMEDISPATCHER_HXX__
#define __SALOMEDISPATCHER_HXX__

#include "Dispatcher.hxx"
#include "yacsgui.hh"
#include "Thread.hxx"
#include "Semaphore.hxx"

#include <string>
#include <list>
#include <map>
#include <set>

namespace YACS
{
  namespace ENGINE
  {
    class SALOMEDispatcher: public Dispatcher
    {
    public:
      SALOMEDispatcher();
      void dispatch(Node* object, const std::string& event);
      void addObserver(YACSGui_ORB::Observer_ptr observer,int numid, std::string event);
      static void setSALOMEDispatcher();
      static SALOMEDispatcher* getSALOMEDispatcher();
      virtual ~SALOMEDispatcher();
    protected:
      static std::map< std::pair<int,std::string> , std::set<YACSGui_ORB::Observer_ptr> > _observers;
      static SALOMEDispatcher* _disp;
      static void *ThDisp( void *a);
      static YACS::BASES::Semaphore _s1;
      static std::list< std::pair<Node*,std::string> > _listOfEvents;
    };


  }
}

#endif
