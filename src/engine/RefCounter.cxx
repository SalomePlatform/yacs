#include "RefCounter.hxx"
//#define REFCNT

#include <iostream>
#include <sstream>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace YACS::ENGINE;

unsigned int RefCounter::_totalCnt=0;

void RefCounter::incrRef() const
{
#ifdef REFCNT
  RefCounter::_totalCnt++;
#endif
  _cnt++;
}

bool RefCounter::decrRef()
{
#ifdef REFCNT
  RefCounter::_totalCnt--;
#endif
  bool ret=(--_cnt==0);
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
