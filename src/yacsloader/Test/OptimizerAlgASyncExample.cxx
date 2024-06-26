// Copyright (C) 2006-2024  CEA, EDF
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

#include "OptimizerAlg.hxx"

#include <iostream>
//using namespace YACS::ENGINE;

extern "C"
{
  YACS::ENGINE::OptimizerAlgBase * createOptimizerAlgASyncExample(YACS::ENGINE::Pool * pool);
}

class OptimizerAlgASyncExample : public YACS::ENGINE::OptimizerAlgASync
{
  private:
    YACS::ENGINE::TypeCode *_tcInt;
    YACS::ENGINE::TypeCode *_tcDouble;
  public:
    OptimizerAlgASyncExample(YACS::ENGINE::Pool *pool);
    virtual ~OptimizerAlgASyncExample();
    
    //! returns typecode of type expected as Input. OwnerShip of returned pointer is held by this.
    virtual YACS::ENGINE::TypeCode *getTCForIn() const;
    //! returns typecode of type expected as Output. OwnerShip of returned pointer is held by this.
    virtual YACS::ENGINE::TypeCode *getTCForOut() const;
    //! returns typecode of type expected for algo initialization. OwnerShip of returned pointer is held by this.
    virtual YACS::ENGINE::TypeCode *getTCForAlgoInit() const;
    //! returns typecode of type expected as algo result. OwnerShip of returned pointer is held by this.
    virtual YACS::ENGINE::TypeCode *getTCForAlgoResult() const;
    virtual void initialize(const YACS::ENGINE::Any *input);
    virtual void startToTakeDecision();
    virtual void finish();//! Called when optimization has succeed.
    virtual YACS::ENGINE::Any * getAlgoResult();
};

OptimizerAlgASyncExample::OptimizerAlgASyncExample(YACS::ENGINE::Pool *pool)
  : YACS::ENGINE::OptimizerAlgASync(pool), _tcInt(0), _tcDouble(0)
{
  _tcDouble = new YACS::ENGINE::TypeCode(YACS::ENGINE::Double);
  _tcInt    = new YACS::ENGINE::TypeCode(YACS::ENGINE::Int);
}

OptimizerAlgASyncExample::~OptimizerAlgASyncExample()
{
  _tcDouble->decrRef();
  _tcInt->decrRef();
}

//! Return the typecode of the expected input of the internal node
YACS::ENGINE::TypeCode * OptimizerAlgASyncExample::getTCForIn() const
{
  return _tcDouble;
}

//! Return the typecode of the expected output of the internal node
YACS::ENGINE::TypeCode * OptimizerAlgASyncExample::getTCForOut() const
{
  return _tcInt;
}

//! Return the typecode of the expected input of the algorithm (algoInit port)
YACS::ENGINE::TypeCode * OptimizerAlgASyncExample::getTCForAlgoInit() const
{
  return _tcInt;
}

//! Return the typecode of the expected output of the algorithm (algoResult port)
YACS::ENGINE::TypeCode * OptimizerAlgASyncExample::getTCForAlgoResult() const
{
  return _tcInt;
}

//! Optional method to initialize the algorithm.
/*!
 *  For now, the parameter input is always NULL. It might be used in the
 *  future to initialize an algorithm with custom data.
 */
void OptimizerAlgASyncExample::initialize(const YACS::ENGINE::Any *input)
{
  std::cout << "Algo initialize, input = " << input->getIntValue() << std::endl;
}

//! This method is called only once to launch the algorithm.
/*!
 *  It must first fill the pool with samples to evaluate and call
 *  signalMasterAndWait() to block until a sample has been evaluated. When
 *  returning from this method, it MUST check for an eventual termination
 *  request (with the method isTerminationRequested()). If the termination
 *  is requested, the method must perform any necessary cleanup and return
 *  as soon as possible. Otherwise it can either add new samples to evaluate
 *  in the pool, do nothing (wait for more samples), or empty the pool and
 *  return to finish the evaluation.
 */
void OptimizerAlgASyncExample::startToTakeDecision()
{
  std::cout << "startToTakeDecision" << std::endl;
  int iter = 0;
  YACS::ENGINE::Any *val=YACS::ENGINE::AtomAny::New(0.5);
  _pool->pushInSample(iter, val);
  
  signalMasterAndWait();
  while(!isTerminationRequested())
  {
    int currentId = _pool->getCurrentId();
    double valIn  = _pool->getCurrentInSample()->getDoubleValue();
    int valOut    = _pool->getCurrentOutSample()->getIntValue();
    
    std::cout << "Compute currentId=" << currentId;
    std::cout << ", valIn=" << valIn;
    std::cout << ", valOut=" << valOut << std::endl;
    
    iter++;
    if(iter < 3)
    {
      YACS::ENGINE::Any *val=YACS::ENGINE::AtomAny::New(valIn + 1);
      _pool->pushInSample(iter, val);
    }
    signalMasterAndWait();
  }
}

/*!
 *  Optional method called when the algorithm has finished, successfully or
 *  not, to perform any necessary clean up.
 */
void OptimizerAlgASyncExample::finish()
{
  std::cout << "Algo finish" << std::endl;
  _pool->destroyAll();
}

/*!
 *  Return the value of the algoResult port.
 */
YACS::ENGINE::Any * OptimizerAlgASyncExample::getAlgoResult()
{
  YACS::ENGINE::Any *val=YACS::ENGINE::AtomAny::New(42);
  return val;
}

//! Factory method to create the algorithm.
YACS::ENGINE::OptimizerAlgBase * createOptimizerAlgASyncExample(YACS::ENGINE::Pool *pool)
{
  return new OptimizerAlgASyncExample(pool);
}
