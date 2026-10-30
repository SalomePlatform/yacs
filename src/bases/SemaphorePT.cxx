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

#include "SemaphorePT.hxx"

using namespace YACS::BASES;

#if defined(__APPLE__)

SemaphorePT::SemaphorePT(int initValue):_value(initValue)
{
  pthread_mutex_init(&_mutex, NULL);
  pthread_cond_init(&_cond, NULL);
}

SemaphorePT::~SemaphorePT()
{
  pthread_cond_destroy(&_cond);
  pthread_mutex_destroy(&_mutex);
}

void SemaphorePT::post()
{
  pthread_mutex_lock(&_mutex);
  _value++;
  pthread_cond_signal(&_cond);
  pthread_mutex_unlock(&_mutex);
}

void SemaphorePT::wait()
{
  pthread_mutex_lock(&_mutex);
  while(_value <= 0)
    pthread_cond_wait(&_cond, &_mutex);
  _value--;
  pthread_mutex_unlock(&_mutex);
}

int SemaphorePT::getValue()
{
  pthread_mutex_lock(&_mutex);
  int val = _value;
  pthread_mutex_unlock(&_mutex);
  return val;
}

#else

SemaphorePT::SemaphorePT(int initValue)
{
  sem_init(&_semDesc, 0, initValue);
}

SemaphorePT::~SemaphorePT()
{
  sem_destroy(&_semDesc);
}

void SemaphorePT::post()
{
  sem_post(&_semDesc);
}

void SemaphorePT::wait()
{
  sem_wait(&_semDesc);
}

int SemaphorePT::getValue()
{
  int val;
  sem_getvalue(&_semDesc,&val);
  return val;
}

#endif
