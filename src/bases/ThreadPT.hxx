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

#ifndef __THREADPT_HXX__
#define __THREADPT_HXX__

#include "YACSBasesExport.hxx"

#include <pthread.h>

namespace YACS
{
  namespace BASES
  {
    class YACSBASES_EXPORT ThreadPT
    {
    public:
      typedef void *(*ThreadJob)(void*);
    public:
      ThreadPT();
      ThreadPT(ThreadJob funcPtr, void *stack, size_t stackSize = 0);
      void go(ThreadJob funcPtr, void *stack, size_t stackSize = 0);
      bool operator==(const ThreadPT& other);
      void join();
      static void detach();
      static void exit(void *what);
      static void sleep(unsigned long usec);
    private:
      pthread_t _threadId;
    };
  }
}

#endif
