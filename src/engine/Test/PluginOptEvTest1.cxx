#include "PluginOptEvTest1.hxx"
#include "TypeCode.hxx"
#include "Pool.hxx"

#include <cmath>

using namespace YACS::ENGINE;

PluginOptEvTest1::PluginOptEvTest1(Pool *pool):OptimizerAlgSync(pool),_tcIn(0),_tcOut(0),_idTest(0)
{
  _tcIn=new TypeCode(Double);
  _tcOut=new TypeCode(Int);
}

PluginOptEvTest1::~PluginOptEvTest1()
{
  _tcIn->decrRef();
  _tcOut->decrRef();
}

TypeCode *PluginOptEvTest1::getTCForIn() const
{
  return _tcIn;
}

TypeCode *PluginOptEvTest1::getTCForOut() const
{
  return _tcOut;
}

void PluginOptEvTest1::parseFileToInit(const std::string& fileName)
{
}

void PluginOptEvTest1::start()
{
  _idTest=0;
  Any *val=AtomAny::New(1.2);
  _pool->pushInSample(4,val);
  val=AtomAny::New(3.4);
  _pool->pushInSample(9,val);
}

void PluginOptEvTest1::takeDecision()
{
  if(_idTest==1)
    {
      Any *val=AtomAny::New(5.6);
      _pool->pushInSample(16,val);
      val=AtomAny::New(7.8);
      _pool->pushInSample(25,val);
      val=AtomAny::New(9. );
      _pool->pushInSample(36,val);
      val=AtomAny::New(12.3);
      _pool->pushInSample(49,val);
    }
  else if(_idTest==4)
    {
      Any *val=AtomAny::New(45.6);
      _pool->pushInSample(64,val);
      val=AtomAny::New(78.9);
      _pool->pushInSample(81,val);
    }
  else
    {
      Any *tmp= _pool->getCurrentInSample();
      if(fabs(tmp->getDoubleValue()-45.6)<1.e-12)
        _pool->destroyAll();
    }
  _idTest++;
}

void PluginOptEvTest1::initialize(const Any *input) throw (Exception)
{
}

void PluginOptEvTest1::finish()
{
}

OptimizerAlgBase *PluginOptEvTest1Factory(Pool *pool)
{
  return new PluginOptEvTest1(pool);
}
