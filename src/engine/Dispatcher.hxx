#ifndef __DISPATCHER_HXX__
#define __DISPATCHER_HXX__

#include <set>
#include <string>
#include <map>

namespace YACS
{
  namespace ENGINE
  {
    class Node;
    class Executor;

/*! \brief Base class for observer in observer pattern
 *
 * When an event occurs, a registered observer is notified by
 * calling its notifyObserver method with 2 arguments : the object
 * which has emitted the event and the event type. There is no more
 * data. It's the responsability of the observer to request all needed
 * information from the emitting object.
 *
 */
    class Observer
    {
    public:
      virtual void notifyObserver(Node* object,const std::string& event);
      virtual ~Observer();
    };

/*! \brief Base class for dispatcher in observer pattern
 *
 * Dispatcher and Observer objects can be used to be notified about events
 * that occurs in editing or executing a calculation schema.
 *
 * When an object wants to notify an event, it calls the dispatch method
 * of the dispatcher with 2 arguments : the object reference and an event type.
 * The dispatcher which is a singleton is obtained by calling the class method
 * getDispatcher.
 * The dispatcher notifies all the registered observers by calling their notifyObserver
 * method.
 *
 * Observers can be registered by calling the addObserver method with two
 * arguments : an object reference and an event type. This observer will be
 * notify with events coming only from this object.
 *
 * Limitation : emitting objects can be only Node
 *
 */
    class Dispatcher
    {
    public:
      virtual void dispatch(Node* object,const std::string& event);
      virtual void addObserver(Observer* observer,Node* object,const std::string& event);
      virtual void removeObserver(Observer* observer,Node* object,const std::string& event);
      virtual void printObservers();
      static Dispatcher* getDispatcher();
      static void setDispatcher(Dispatcher* dispatcher);
      virtual ~Dispatcher();
    protected:
      std::map< std::pair<Node*,std::string> , std::set<Observer*> > _observers;
      std::map< std::pair<Executor*,std::string> , std::set<Observer*> > _observExec;
      static Dispatcher* _singleton;
    };

  }
}

#endif
