#ifndef __POOL_HXX__
#define __POOL_HXX__

#include "Exception.hxx"

#include <list>

namespace YACS
{
  namespace ENGINE
  {
    class Any;
    class OptimizerLoop;

    class Pool
    {
      friend class OptimizerLoop;

      class ExpData
      {
      private:
        Any *_in;
        Any *_out;
        unsigned char _priority;
      public:
        ExpData(Any *inValue, unsigned char priority);
        ExpData(const ExpData& other);
        ~ExpData();
        Any *inValue() const { return _in; }
        Any *outValue() const { return _out; }
        void setOutValue(Any *outValue);
        void markItAsInUse();
        bool isLaunchable() const;
        unsigned char getPriority() const { return _priority; }
      private:
        static Any *NOT_USED_NOR_COMPUTED;
        static Any *USED_BUT_NOT_COMPUTED_YET;
      };
    private:
      std::list< std::pair<int, ExpData> > _container;
      std::list< std::pair<int, ExpData> >::iterator _currentCase;
    public:
      //For algorithm use
      int getCurrentId() const { return (*_currentCase).first; }
      Any *getCurrentInSample() const { return (*_currentCase).second.inValue(); }
      Any *getCurrentOutSample() const { return (*_currentCase).second.outValue(); }
      void pushInSample(int id, Any *inSample, unsigned char priority = 0);
      void destroyAll();
    private:
      //For OptimizerNode use
      void destroyCurrentCase();
      void checkConsistency() throw(Exception);
      void setCurrentId(int id) throw(Exception);
      void putOutSampleAt(int id, Any *outValue) throw(Exception);
      Any *getNextSampleWithHighestPriority(int& id, unsigned char& priority) const;
      void markIdAsInUse(int id);
      bool empty() const;
    private:
      static const char MESSAGEFORUNXSTNGID[];
    };
  }
}

#endif
