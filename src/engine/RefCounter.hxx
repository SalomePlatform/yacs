#ifndef __REFCOUNTER_HXX__
#define __REFCOUNTER_HXX__

namespace YACS
{
  namespace ENGINE
  {
    class RefCounter
    {
    public:
      unsigned int getRefCnt() const { return _cnt; }
      void incrRef() const;
      bool decrRef();
      static unsigned int _totalCnt;
    protected:
      RefCounter();
      virtual ~RefCounter();
    protected:
      mutable unsigned int _cnt;
    };
  }
}

#endif
