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

#include "OptimizerAlg.hxx"

using namespace YACS::ENGINE;

extern "C"
{
  OptimizerAlgBase * createOptimizerAlgASyncExample(Pool * pool);
}

class OptimizerAlgASyncExample : public OptimizerAlgASync
  {
  private:
    TypeCode * _tcIn;
    TypeCode * _tcOut;
  public:
    OptimizerAlgASyncExample(Pool * pool);
    virtual ~OptimizerAlgASyncExample();
    TypeCode * getTCForIn() const;
    TypeCode * getTCForOut() const;
    void startToTakeDecision();
  };

OptimizerAlgASyncExample::OptimizerAlgASyncExample(Pool * pool) : OptimizerAlgASync(pool),
                                                                  _tcIn(0), _tcOut(0)
{
  _tcIn = new TypeCode(Double);
  _tcOut = new TypeCode(Int);
}

OptimizerAlgASyncExample::~OptimizerAlgASyncExample()
{
  _tcIn->decrRef();
  _tcOut->decrRef();
}

//! Return the typecode of the expected input type
TypeCode *OptimizerAlgASyncExample::getTCForIn() const
{
  return _tcIn;
}

//! Return the typecode of the expected output type
TypeCode *OptimizerAlgASyncExample::getTCForOut() const
{
  return _tcOut;
}

//! This method is called only once to launch the algorithm.
/*!
 *  It must first fill the pool with samples to evaluate and call signalMasterAndWait()
 *  to block until a sample has been evaluated. When returning from this method, it MUST
 *  check for an eventual termination request (with the method isTerminationRequested()).
 *  If the termination is requested, the method must perform any necessary cleanup and
 *  return as soon as possible. Otherwise it can either add new samples to evaluate in
 *  the pool, do nothing (wait for more samples), or empty the pool and return to finish
 *  the evaluation.
 */
void OptimizerAlgASyncExample::startToTakeDecision()
{
  double val = 1.2;
  for (int i=0 ; i<5 ; i++) {
    // push a sample in the input of the slave node
    _pool->pushInSample(i, AtomAny::New(val));
    // wait until next sample is ready
    signalMasterAndWait();
    // check error notification
    if (isTerminationRequested()) {
      _pool->destroyAll();
      return;
    }

    // get a sample from the output of the slave node
    Any * v = _pool->getCurrentOutSample();
    val += v->getIntValue();
  }

  // in the end destroy the pool content
  _pool->destroyAll();
}

//! Factory method to create the algorithm.
OptimizerAlgBase * createOptimizerAlgASyncExample(Pool * pool)
{
  return new OptimizerAlgASyncExample(pool);
}
