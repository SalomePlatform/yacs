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
#ifndef __PYOPTIMIZERALG_HXX__
#define __PYOPTIMIZERALG_HXX__

#include "YACSRuntimeSALOMEExport.hxx"
#include "OptimizerAlg.hxx"

namespace YACS
{
  namespace ENGINE
  {
    /*!
     *  \brief Base class factorizing common methods for python algorithms interfaces.
     */
    class YACSRUNTIMESALOME_EXPORT PyOptimizerAlgBase : public OptimizerAlgBase
    {
    public:
      virtual ~PyOptimizerAlgBase();

      virtual TypeCode *getTCForInProxy() const;
      virtual TypeCode *getTCForOutProxy() const;
      virtual TypeCode *getTCForAlgoInitProxy() const;
      virtual TypeCode *getTCForAlgoResultProxy() const;
      virtual void initializeProxy(const Any *input) throw (Exception);
      virtual void startProxy();
      virtual void takeDecisionProxy();
      virtual void finishProxy();
      virtual Any * getAlgoResultProxy();

    protected:
      PyOptimizerAlgBase(Pool *pool);
    };

    typedef PyOptimizerAlgBase PyOptimizerAlgSync;

    /*!
     *  \brief Base class to implement in external dynamic lib or external python module
     *  in case of a non event oriented optimizer using Python code.
     */
    class YACSRUNTIMESALOME_EXPORT PyOptimizerAlgASync : public PyOptimizerAlgBase,
                                                         public YACS::BASES::AlternateThread
    {
    public:
      virtual ~PyOptimizerAlgASync();

      virtual void startProxy();
      virtual void takeDecisionProxy();
      virtual void finishProxy();//! Called when optimization has succeed.
      virtual void signalMasterAndWait();

    protected:
      PyOptimizerAlgASync(Pool *pool);

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
  }
}

#endif
