#ifndef __YACS_CppCOMPONENT__
#define __YACS_CppCOMPONENT__

#include <string>
#include "Any.hxx"
#include "ComponentInstance.hxx"

namespace YACS 
{
  namespace ENGINE
  {

    struct returnInfo {
         int code;
         std::string message;
    };

    typedef void * (*InitFunction)();
    typedef void (*RunFunction) (void *, const char *, int, int, Any **, Any **, returnInfo *);
    typedef void (*TerminateFunction)(void **);
    typedef void (*PingFunction) ();
        
    class CppComponent : public ComponentInstance {
      public:

        CppComponent(const std::string & name);
        CppComponent(void * obj, RunFunction r, TerminateFunction t, 
		               const std::string & name) 
             : __obj(obj), __run(r), __terminate(t), ComponentInstance(name) {}
        CppComponent(const CppComponent& other);
        virtual ~CppComponent();

        void run(const char * service, int nbIn, int nbOut,
                         Any ** argIn, Any ** argOut) throw (YACS::Exception);

        static const char KIND[];
        virtual std::string getKind() const;
        virtual void load();
        virtual void unload();
        virtual bool isLoaded();
        virtual ServiceNode* createNode(const std::string& name);
        virtual YACS::ENGINE::ComponentInstance* clone() const;
        
      protected:
  
        void * __obj;
        YACS::ENGINE::RunFunction __run;
        YACS::ENGINE::TerminateFunction __terminate;
    };
  };
};

#endif
