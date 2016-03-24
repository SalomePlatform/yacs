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

#ifndef __POOL_HXX__
#define __POOL_HXX__

#include "YACSlibEngineExport.hxx"
#include "Exception.hxx"

#include <list>

namespace YACS
{
  namespace ENGINE
  {
    class Any;
    class OptimizerLoop;

    /*! \brief Pool used to manage the samples of the optimizer loop plugin.
     *  
     *  Every sample has an identifier (Id), a priority, an initial value (In)
     *  and an evaluation value (Out).
     *  The current sample is the sample used by the latest terminated evaluation.
     */
    class YACSLIBENGINE_EXPORT Pool
    {
      friend class OptimizerLoop;

      class ExpData
      {
      private:
        Any *_in;
        Any *_out;
        unsigned char _priority;
      public:
        ExpData(Any *inValue, unsigned char priority);
        ExpData(const ExpData& other);
        ~ExpData();
        Any *inValue() const { return _in; }
        Any *outValue() const { return _out; }
        void setOutValue(Any *outValue);
        void markItAsInUse();
        bool isLaunchable() const;
        unsigned char getPriority() const { return _priority; }
      private:
        static Any *NOT_USED_NOR_COMPUTED;
        static Any *USED_BUT_NOT_COMPUTED_YET;
      };
    private:
      std::list< std::pair<int, ExpData> > _container;
      std::list< std::pair<int, ExpData> >::iterator _currentCase;
    public:
      //For algorithm use
      int getCurrentId() const ;
      Any *getCurrentInSample() const ;
      Any *getCurrentOutSample() const ;
      Any *getOutSample(int id);
      void pushInSample(int id, Any *inSample, unsigned char priority = 0);
      void destroyAll();
    private:
      //For OptimizerNode use
      void destroyCurrentCase();
      void checkConsistency() throw(Exception);
      void setCurrentId(int id) throw(Exception);
      void putOutSampleAt(int id, Any *outValue) throw(Exception);
      Any *getNextSampleWithHighestPriority(int& id, unsigned char& priority) const;
      void markIdAsInUse(int id);
      bool empty() const;
    private:
      static const char MESSAGEFORUNXSTNGID[];
    };
  }
}

#endif
