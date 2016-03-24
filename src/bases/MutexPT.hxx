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

#ifndef __MUTEXPT_HXX__
#define __MUTEXPT_HXX__

#include "YACSBasesExport.hxx"

#include <pthread.h>

namespace YACS
{
  namespace BASES
  {
    class ConditionPT;

    class YACSBASES_EXPORT MutexPT
    {
    public:
      MutexPT();
      ~MutexPT();
      void lock();
      void unLock();
      friend class ConditionPT;
    private:
      pthread_mutex_t _mutexDesc;
      pthread_mutexattr_t _options;

    };

    class YACSBASES_EXPORT ConditionPT
    {
    public:
      ConditionPT();
      ~ConditionPT();
      void notify_one();
      void notify_all();
      void wait(MutexPT& mutex);
    private:
      pthread_cond_t _cond;
    };
  }
}

#endif
