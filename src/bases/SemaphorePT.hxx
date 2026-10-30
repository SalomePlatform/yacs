// Copyright (C) 2006-2026  CEA, EDF
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

#ifndef __SEMAPHOREPT_HXX__
#define __SEMAPHOREPT_HXX__

#include "YACSBasesExport.hxx"

#if defined(__APPLE__)
// macOS: unnamed POSIX semaphores (sem_init) are not implemented,
// use a counting semaphore built on a mutex and a condition variable
#include <pthread.h>
#else
#include <semaphore.h>
#endif

namespace YACS
{
  namespace BASES
  {
    class YACSBASES_EXPORT SemaphorePT
    {
    public:
      SemaphorePT(int initValue=0);
      ~SemaphorePT();
      void post();
      void wait();
      int getValue();
    private:
#if defined(__APPLE__)
      pthread_mutex_t _mutex;
      pthread_cond_t _cond;
      int _value;
#else
      sem_t _semDesc;
#endif
    };
  }
}

#endif
