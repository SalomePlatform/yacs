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

#include "MutexPT.hxx"

using namespace YACS::BASES;

MutexPT::MutexPT()
{
  //pthread_mutexattr_settype(&_options, PTHREAD_MUTEX_FAST_NP);
  //pthread_mutex_init(&_mutexDesc, &_options);
  pthread_mutex_init(&_mutexDesc, NULL); // potential hang up at start with commented init
}

MutexPT::~MutexPT()
{
  pthread_mutex_destroy(&_mutexDesc);
}

void MutexPT::lock()
{
  pthread_mutex_lock(&_mutexDesc);
}

void MutexPT::unLock()
{
  pthread_mutex_unlock(&_mutexDesc);
}

ConditionPT::ConditionPT()
{
  pthread_cond_init(&_cond,  NULL);
}

ConditionPT::~ConditionPT()
{
  pthread_cond_destroy(&_cond);
}

void ConditionPT::notify_one()
{
  pthread_cond_signal(&_cond);
}

void ConditionPT::notify_all()
{
  pthread_cond_broadcast(&_cond);
}

void ConditionPT::wait(MutexPT& mutex)
{
  pthread_cond_wait(&_cond, &mutex._mutexDesc);
}

