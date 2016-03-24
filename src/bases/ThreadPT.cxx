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

#include "ThreadPT.hxx"
#include "Exception.hxx"
#ifdef WIN32
#include <windows.h>
#define usleep(A) Sleep(A/1000)
#else
#include <unistd.h>
#endif

using namespace YACS::BASES;

ThreadPT::ThreadPT()
{
}

ThreadPT::ThreadPT(ThreadJob funcPtr, void *stack, size_t stackSize)
{
  go(funcPtr,stack,stackSize);
}

void ThreadPT::go(ThreadJob funcPtr, void *stack, size_t stackSize)
{
  int err;
  void **stackT=(void **) stack;
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  if (stackSize > 0)
    {
      err = pthread_attr_setstacksize(&attr, stackSize);
      if (err != 0) throw Exception("Error when setting thread stack size");
    }
  err = pthread_create(&_threadId, &attr, funcPtr, stackT);
  pthread_attr_destroy(&attr);
  if(err!=0)throw Exception("Error in thread creation");
}

bool ThreadPT::operator==(const ThreadPT& other)
{
  return pthread_equal(_threadId, other._threadId) != 0;
}

//! Detach thread to release resources on exit
void ThreadPT::detach()
{
  pthread_detach(pthread_self());
}

void ThreadPT::exit(void *what)
{
  pthread_exit(what);
}

void ThreadPT::join()
{
  void *ret;
  pthread_join(_threadId, &ret);
}

void ThreadPT::sleep(unsigned long usec)
{
  usleep(usec);
}
