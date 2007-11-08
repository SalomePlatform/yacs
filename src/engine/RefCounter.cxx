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
  _globalMutexForTS.unlock();
}

bool RefCounter::decrRef()
{
  _globalMutexForTS.lock();
#ifdef REFCNT
  RefCounter::_totalCnt--;
#endif
  bool ret=(--_cnt==0);
  _globalMutexForTS.unlock();
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
