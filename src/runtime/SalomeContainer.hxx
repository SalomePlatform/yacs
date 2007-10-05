#ifndef __SALOMECONTAINER_HXX__
#define __SALOMECONTAINER_HXX__

#include "Container.hxx"
#include "Mutex.hxx"
#include <SALOMEconfig.h>
#include CORBA_CLIENT_HEADER(SALOME_Component)

namespace YACS
{
  namespace ENGINE
  {
    class SalomeComponent;

    class SalomeContainer : public Container
    {
      friend class SalomeComponent;
    public:
      SalomeContainer();
      SalomeContainer(const SalomeContainer& other);
      //! For thread safety for concurrent load operation on same Container.
      void lock();
      //! For thread safety for concurrent load operation on same Container.
      void unLock();
      bool isAlreadyStarted() const;
      void start() throw (Exception);
      Container *clone() const;
      std::string getPlacementId() const;
      void checkCapabilityToDealWith(const ComponentInstance *inst) const throw (Exception);
    protected:
      virtual ~SalomeContainer();
    protected:
      //! thread safety in Salome ???
      YACS::BASES::Mutex _mutex;
      Engines::Container_var _trueCont;
    };
  }
}

#endif
