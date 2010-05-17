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

#include "DrivenConditionPT.hxx"

using namespace YACS::BASES;

DrivenConditionPT::DrivenConditionPT()
{
  pthread_mutex_init(&_mutexDesc1, NULL);
  pthread_mutex_init(&_mutexDesc2, NULL);
  pthread_cond_init(&_cond1, NULL);
  pthread_cond_init(&_cond2, NULL);
  pthread_mutex_lock(&_mutexDesc1);
  pthread_mutex_lock(&_mutexDesc2);
}

DrivenConditionPT::~DrivenConditionPT()
{
  pthread_cond_destroy(&_cond1);
  pthread_cond_destroy(&_cond2);
  pthread_mutex_destroy(&_mutexDesc1);
  pthread_mutex_destroy(&_mutexDesc2);
}

void DrivenConditionPT::notifyOneSync()
{
  pthread_mutex_lock(&_mutexDesc1);
  pthread_cond_signal(&_cond1);
  pthread_mutex_unlock(&_mutexDesc1);
  pthread_cond_wait(&_cond2, &_mutexDesc2);
}

void DrivenConditionPT::waitForAWait()
{
  pthread_cond_wait(&_cond2, &_mutexDesc2);
}

void DrivenConditionPT::wait()
{
  pthread_mutex_lock(&_mutexDesc2);
  pthread_cond_signal(&_cond2);
  pthread_mutex_unlock(&_mutexDesc2);
  pthread_cond_wait(&_cond1,&_mutexDesc1);
}

void DrivenConditionPT::signal()
{
  pthread_mutex_lock(&_mutexDesc2);
  pthread_cond_signal(&_cond2);
  pthread_mutex_unlock(&_mutexDesc2);
}
