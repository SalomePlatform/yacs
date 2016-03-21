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

#include "RefCounter.hxx"

#include "Mutex.hxx"

//#define REFCNT

#include <iostream>
#include <sstream>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace YACS::ENGINE;

unsigned int RefCounter::_totalCnt=0;

static YACS::BASES::Mutex _globalMutexForTS;

void RefCounter::incrRef() const
{
  _globalMutexForTS.lock();
#ifdef REFCNT
  RefCounter::_totalCnt++;
#endif
  _cnt++;
  _globalMutexForTS.unLock();
}

bool RefCounter::decrRef()
{
  _globalMutexForTS.lock();
#ifdef REFCNT
  RefCounter::_totalCnt--;
#endif
  bool ret=(--_cnt==0);
  _globalMutexForTS.unLock();
  if(ret)
    delete this;
  return ret;
}

RefCounter::RefCounter():_cnt(1)
{
#ifdef REFCNT
  RefCounter::_totalCnt++;
#endif
}

RefCounter::RefCounter(const RefCounter& other):_cnt(1)
{
}

RefCounter::~RefCounter()
{
#ifdef REFCNT
  if(_cnt > 0)
    {
      DEBTRACE("Ref count > 0: " << this << " " << _cnt);
      AttachDebugger();
    }
#endif
}
