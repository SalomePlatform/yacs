#ifndef __PUGINOPTEVTEST1_HXX__
#define __PUGINOPTEVTEST1_HXX__

#include "OptimizerAlg.hxx"

extern "C"
{
  YACS::ENGINE::OptimizerAlgBase *PluginOptEvTest1Factory(YACS::ENGINE::Pool *pool);
}

namespace YACS
{
  namespace ENGINE
  {
    class PluginOptEvTest1 : public OptimizerAlgSync
    {
    private:
      int _idTest;
      TypeCode *_tcIn;
      TypeCode *_tcOut;
    public:
      PluginOptEvTest1(Pool *pool);
      virtual ~PluginOptEvTest1();
      TypeCode *getTCForIn() const;
      TypeCode *getTCForOut() const;
      void parseFileToInit(const std::string& fileName);
      void start();
      void takeDecision();
      void initialize(const Any *input) throw(Exception);
      void finish();
    };
  }
}

#endif
