#include "OptimizerAlg.hxx"

using namespace YACS::ENGINE;

OptimizerAlgBase::OptimizerAlgBase(Pool *pool):_pool(pool)
{
}

OptimizerAlgBase::~OptimizerAlgBase()
{
}

OptimizerAlgSync::OptimizerAlgSync(Pool *pool):OptimizerAlgBase(pool)
{
}

OptimizerAlgSync::~OptimizerAlgSync()
{
}

TypeOfAlgInterface OptimizerAlgSync::getType() const
{ 
  return EVENT_ORIENTED; 
}

OptimizerAlgASync::OptimizerAlgASync(Pool *pool):OptimizerAlgBase(pool)
{
}

OptimizerAlgASync::~OptimizerAlgASync()
{
}

TypeOfAlgInterface OptimizerAlgASync::getType() const
{ 
  return NOT_EVENT_ORIENTED;
}
