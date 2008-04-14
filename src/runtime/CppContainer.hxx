#ifndef __Cpp_CONTAINER_HXX__
#define __Cpp_CONTAINER_HXX__

#include <map>
#include <string>
#include "Mutex.hxx"
#include "Container.hxx"
#include "CppComponent.hxx"

namespace YACS
{
  namespace ENGINE
  {
  
    struct LocalLibrary {
      
      void * handle;
      
      InitFunction initHandle;
      RunFunction runHandle;
      PingFunction pingHandle;
      TerminateFunction terminateHandle;
      
      LocalLibrary(void *h, InitFunction i, RunFunction r, 
                            PingFunction p, TerminateFunction t) 
	      : handle(h), initHandle(i), runHandle(r), 
                           pingHandle(p), terminateHandle(t) {}
      LocalLibrary() 
	      : handle(NULL), initHandle(NULL), runHandle(NULL), 
                              pingHandle(NULL), terminateHandle(NULL) {}
                              
      bool good() {
        return (handle != NULL)    && (initHandle != NULL) && 
               (runHandle != NULL) && (terminateHandle != NULL);
      }
        
    };

    // Local container singleton
    class LocalContainer {

      friend class CppComponent;

    public:

      static YACS::ENGINE::LocalContainer * get();
      void destroy();
      LocalLibrary loadComponentLibrary(const std::string &, const char * prefix = NULL, 
                                        bool forcedLoad = false);
      CppComponent * createComponentInstance(const char * componentName);
      void createInternalInstance(const char * componentName, 
    		                      void *& obj, RunFunction &r, TerminateFunction &t);
     void unLoadComponentLibrary(const std::string & aCompName);
      void unregisterComponentInstance(CppComponent * C);
                  
    protected:

      LocalContainer();
      virtual ~LocalContainer();

      YACS::BASES::Mutex _instance_mapMutex, _library_mapMutex;
      static std::map<std::string, LocalLibrary > _library_map; // libraries, loaded
      static std::multimap<std::string, CppComponent *>  _instance_map;
      
      
    private:
      static LocalContainer *_singleton;
      

    };

    class CppContainer : public Container {

      friend class CppComponent;
      friend class LocalContainer;
      
    public:
 
      CppContainer();
      virtual ~CppContainer();
      bool isAlreadyStarted() const;
      void start() throw (YACS::Exception);
      std::string getPlacementId() const;
      YACS::ENGINE::Container *clone() const;

      void lock();
      void unLock();
       
      void checkCapabilityToDealWith(const ComponentInstance *inst) const throw (YACS::Exception);
      bool loadComponentLibrary(const std::string & componentName) throw (YACS::Exception);
      CppComponent * createComponentInstance(const std::string & componentName, int studyID = 0);
      void createInternalInstance(const std::string & componentName, 
    		                      void *& obj, RunFunction &r, TerminateFunction &t);
      void unregisterComponentInstance(CppComponent * C);
      
     protected:
       YACS::BASES::Mutex _mutex;
       LocalContainer * _trueCont;

     };
  };
};

#endif

