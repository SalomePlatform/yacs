
#include "SALOMEDispatcher.hxx"
#include "Node.hxx"

#include <iostream>

using namespace std;
using namespace YACS::BASES;
using namespace YACS::ENGINE;

map< pair<int,string> , set<YACSGui_ORB::Observer_ptr> > SALOMEDispatcher::_observers;
SALOMEDispatcher* SALOMEDispatcher::_disp = 0;

SALOMEDispatcher::SALOMEDispatcher()
{
}

SALOMEDispatcher::~SALOMEDispatcher()
{
}

void SALOMEDispatcher::setSALOMEDispatcher()
{
  cerr << "SALOMEDispatcher::setSALOMEDispatcher" << endl;
  _disp=new SALOMEDispatcher();
  YACS::ENGINE::Dispatcher::setDispatcher(_disp);
}

SALOMEDispatcher* SALOMEDispatcher::getSALOMEDispatcher()
{
  return _disp;
}

void SALOMEDispatcher::dispatch(Node* object, const std::string& event)
{
  cerr << "SALOMEDispatcher::dispatch " << event << " " << object->getNumId() << endl;
  CORBA::Long numId = object->getNumId();
  typedef set<YACSGui_ORB::Observer_ptr>::iterator jt;
  std::pair<int,std::string> key(numId, event);
  for(jt iter = _observers[key].begin(); iter!=_observers[key].end(); iter++)
    {
      if (! CORBA::is_nil(*iter))
        {
          cerr << "numId, event " << numId << " " << event << endl;
          (*iter)->notifyObserver(numId, event.c_str());
        }
      else
        cerr << "************************** dispatch on a CORBA::nil *******************************" <<endl;
    }
}

void SALOMEDispatcher::addObserver(YACSGui_ORB::Observer_ptr observer,
                                   int numid,
                                   std::string event)
{
  _observers[std::pair<int,std::string>(numid,event)].insert(YACSGui_ORB::Observer::_duplicate(observer));
}
