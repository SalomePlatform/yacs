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

#ifndef __OPTIMIZERALG_HXX__
#define __OPTIMIZERALG_HXX__

#include <string>

#include "YACSlibEngineExport.hxx"
#include "RefCounter.hxx"
#include "Exception.hxx"
#include "AlternateThread.hxx"
#include "Pool.hxx"
#include "TypeCode.hxx"

namespace YACS
{
  namespace ENGINE
  {
    class Proc;

    /*!
     *  \brief Base class factorizing common methods for all algorithms interfaces.
     */
    class YACSLIBENGINE_EXPORT OptimizerAlgBase : public RefCounter
    {
    protected:
      Pool *_pool;
      Proc * _proc;
      std::string _errorMessage;
      int _nbOfBranches;

    protected:
      OptimizerAlgBase(Pool *pool);
      virtual ~OptimizerAlgBase();

      // Methods to implement in real algorithm classes. These methods shall be called
      // only through proxy methods.
      //! returns typecode of type expected as Input. OwnerShip of returned pointer is held by this.
      virtual TypeCode *getTCForIn() const = 0;
      //! returns typecode of type expected as Output. OwnerShip of returned pointer is held by this.
      virtual TypeCode *getTCForOut() const = 0;
      //! returns typecode of type expected for algo initialization. OwnerShip of returned pointer is held by this.
      virtual TypeCode *getTCForAlgoInit() const;
      //! returns typecode of type expected as algo result. OwnerShip of returned pointer is held by this.
      virtual TypeCode *getTCForAlgoResult() const;
      virtual void initialize(const Any *input) throw (Exception);
      virtual void start() = 0;//! Update _pool attribute before performing anything.
      virtual void takeDecision() = 0;//! _pool->getCurrentId gives the \b id at the origin of this call.
                                      //! Perform the job of analysing to know what new jobs to do (_pool->pushInSample)
                                      //! or in case of convergence _pool->destroyAll
      virtual void finish();//! Called when optimization has succeed.
      virtual Any * getAlgoResult();

    public:
      // Proxy methods to encapsulate the call to real methods
      virtual TypeCode *getTCForInProxy() const;
      virtual TypeCode *getTCForOutProxy() const;
      virtual TypeCode *getTCForAlgoInitProxy() const;
      virtual TypeCode *getTCForAlgoResultProxy() const;
      virtual void initializeProxy(const Any *input) throw (Exception);
      virtual void startProxy();
      virtual void takeDecisionProxy();
      virtual void finishProxy();
      virtual Any * getAlgoResultProxy();

      // Utility methods
      virtual void setPool(Pool *pool);
      virtual void setProc(Proc * proc);
      virtual Proc * getProc();
      virtual bool hasError() const;
      virtual const std::string & getError() const;
      virtual void setError(const std::string & message);
      void setNbOfBranches(int nbOfBranches);
      int getNbOfBranches() const;
    };

    typedef OptimizerAlgBase OptimizerAlgSync;

    /*!
     *  \brief Base class to implement in external dynamic lib in case of optimizer non event oriented.
     */
    class YACSLIBENGINE_EXPORT OptimizerAlgASync : public OptimizerAlgBase,
                                                   public YACS::BASES::AlternateThread
    {
    protected:
      OptimizerAlgASync(Pool *pool);
    public:
      virtual ~OptimizerAlgASync();
      virtual void finishProxy();//! Called when optimization has succeed.

    protected:
      virtual void start();//! Update _pool attribute before performing anything.
      virtual void takeDecision();//! _pool->getCurrentId gives the \b id at the origin of this call.
                                  //! Perform the job of analysing to know what new jobs to do (_pool->pushInSample)
                                  //! or in case of convergence _pool->destroyAll
      virtual void run();

      //! _pool->getCurrentId gives the \b id at the origin of this call.
      //! Perform the job between 2 'condition->wait()' of analysing to know what new jobs to do
      //! (_pool->pushInSample) or in case of convergence _pool->destroyAll
      //! WARNING ! 'condition->wait()' must be called before any analyse of Pool.
      virtual void startToTakeDecision()=0;
    };

    typedef OptimizerAlgBase *(*OptimizerAlgBaseFactory)(Pool *pool);
  }
}

#endif
