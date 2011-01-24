//  Copyright (C) 2006-2008  CEA/DEN, EDF R&D
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

#include <cmath>
#include <iostream>

#include "OptimizerAlg.hxx"

using namespace YACS::ENGINE;

extern "C"
{
  OptimizerAlgBase * createOptimizerAlgSyncExample(Pool * pool);
}

class OptimizerAlgSyncExample : public OptimizerAlgSync
  {
  private:
    int _idTest;
    TypeCode *_tcIn;
    TypeCode *_tcOut;
  public:
    OptimizerAlgSyncExample(Pool *pool);
    virtual ~OptimizerAlgSyncExample();
    TypeCode *getTCForIn() const;
    TypeCode *getTCForOut() const;
    void start();
    void takeDecision();
    void initialize(const Any *input) throw(YACS::Exception);
    void finish();
  };

OptimizerAlgSyncExample::OptimizerAlgSyncExample(Pool *pool) : OptimizerAlgSync(pool),
                                                               _tcIn(0), _tcOut(0),
                                                               _idTest(0)
{
  _tcIn=new TypeCode(Double);
  _tcOut=new TypeCode(Int);
}

OptimizerAlgSyncExample::~OptimizerAlgSyncExample()
{
  std::cout << "Destroying OptimizerAlgSyncExample" << std::endl;
  _tcIn->decrRef();
  _tcOut->decrRef();
  std::cout << "Destroyed OptimizerAlgSyncExample" << std::endl;
}

//! Return the typecode of the expected input type
TypeCode * OptimizerAlgSyncExample::getTCForIn() const
{
  return _tcIn;
}

//! Return the typecode of the expected output type
TypeCode * OptimizerAlgSyncExample::getTCForOut() const
{
  return _tcOut;
}

//! Start to fill the pool with samples to evaluate
void OptimizerAlgSyncExample::start()
{
  _idTest=0;
  Any *val=AtomAny::New(1.2);
  _pool->pushInSample(4,val);
  val=AtomAny::New(3.4);
  _pool->pushInSample(9,val);
}

//! This method is called each time a sample has been evaluated.
/*!
 *  It can either add new samples to evaluate in the pool, do nothing (wait for more
 *  samples), or empty the pool to finish the evaluation.
 */
void OptimizerAlgSyncExample::takeDecision()
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

//! Optional method to initialize the algorithm.
/*!
 *  For now, the parameter input is always NULL. It might be used in the future to
 *  initialize an algorithm with custom data.
 */
void OptimizerAlgSyncExample::initialize(const Any *input) throw (YACS::Exception)
{
}

/*!
 *  Optional method called when the algorithm has finished, successfully or not, to
 *  perform any necessary clean up.
 */
void OptimizerAlgSyncExample::finish()
{
}

//! Factory method to create the algorithm.
OptimizerAlgBase * createOptimizerAlgSyncExample(Pool *pool)
{
  return new OptimizerAlgSyncExample(pool);
}
