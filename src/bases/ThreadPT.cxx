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

#include "ThreadPT.hxx"
#include "Exception.hxx"
#ifdef WNT
#include <windows.h>
#define usleep(A) Sleep(A/1000)
#else
#include <unistd.h>
#endif

using namespace YACS::BASES;

ThreadPT::ThreadPT(ThreadJob funcPtr, void *stack)
{
  int err;
  void **stackT=(void **) stack;
  err=pthread_create(&_threadId,0,funcPtr,stackT);
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
