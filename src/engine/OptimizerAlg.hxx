//  Copyright (C) 2006-2010  CEA/DEN, EDF R&D
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#ifndef __OPTIMIZERALG_HXX__
#define __OPTIMIZERALG_HXX__

#include "YACSlibEngineExport.hxx"
#include "RefCounter.hxx"
#include "Exception.hxx"
#include "DrivenCondition.hxx"

#include <string>

namespace YACS
{
  namespace ENGINE
  {
    class Any;
    class Pool;
    class TypeCode;

    typedef enum
      {
        EVENT_ORIENTED = 26,
        NOT_EVENT_ORIENTED = 28
      } TypeOfAlgInterface;

    /*!
     *  \brief Base class factorizing common methods for all algorithms interfaces.
     */
    class YACSLIBENGINE_EXPORT OptimizerAlgBase : public RefCounter
    {
    protected:
      Pool *_pool;
    protected:
      OptimizerAlgBase(Pool *pool);
      virtual ~OptimizerAlgBase();
    public:
      //! returns typecode of type expected as Input. OwnerShip of returned pointer is held by this.
      virtual TypeCode *getTCForIn() const = 0;
      //! returns typecode of type expected as Output. OwnerShip of returned pointer is held by this.
      virtual TypeCode *getTCForOut() const = 0;
      virtual void parseFileToInit(const std::string& fileName) = 0;
      virtual TypeOfAlgInterface getType() const = 0;
      virtual void initialize(const Any *input) throw (Exception) = 0;
      virtual void finish() = 0;//! Called when optimization has succeed.
      virtual void setPool(Pool *pool);
    };

    /*!
     *  \brief Base class to implement in external dynamic lib in case of optimizer event oriented.
     */
    class YACSLIBENGINE_EXPORT OptimizerAlgSync : public OptimizerAlgBase
    {
    protected:
      OptimizerAlgSync(Pool *pool);
    public:
      TypeOfAlgInterface getType() const;
      virtual ~OptimizerAlgSync();
      virtual void start() = 0;//! Update _pool attribute before performing anything.
      virtual void takeDecision() = 0;//! _pool->getCurrentId gives the \b id at the origin of this call.
                                      //! Perform the job of analysing to know what new jobs to do (_pool->pushInSample)
                                      //! or in case of convergence _pool->destroyAll
    };

    /*!
     *  \brief Base class to implement in external dynamic lib in case of optimizer non event oriented.
     */
    class YACSLIBENGINE_EXPORT OptimizerAlgASync : public OptimizerAlgBase
    {
    protected:
      OptimizerAlgASync(Pool *pool);
    public:
      TypeOfAlgInterface getType() const;
      virtual ~OptimizerAlgASync();
      virtual void startToTakeDecision(::YACS::BASES::DrivenCondition *condition) = 0;//! _pool->getCurrentId gives the \b id at the origin of this call.
      //! Perform the job between 2 'condition->wait()' of analysing to know what new jobs to do
      //! (_pool->pushInSample) or in case of convergence _pool->destroyAll
      //! WARNING ! 'condition->wait()' must be called before any analyse of Pool.
    };

    typedef OptimizerAlgBase *(*OptimizerAlgBaseFactory)(Pool *pool);
  }
}

#endif
