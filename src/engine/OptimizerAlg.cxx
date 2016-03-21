// Copyright (C) 2006-2016  CEA/DEN, EDF R&D
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
#include "Runtime.hxx"

using namespace YACS::BASES;
using namespace YACS::ENGINE;

OptimizerAlgBase::OptimizerAlgBase(Pool *pool):_pool(pool), _proc(NULL)
{
}

OptimizerAlgBase::~OptimizerAlgBase()
{
}

void OptimizerAlgBase::initialize(const Any *input) throw (YACS::Exception)
{
}

void OptimizerAlgBase::finish()
{
}

TypeCode * OptimizerAlgBase::getTCForAlgoInit() const
{
  return Runtime::_tc_string;
}

TypeCode * OptimizerAlgBase::getTCForAlgoResult() const
{
  return Runtime::_tc_string;
}

Any * OptimizerAlgBase::getAlgoResult()
{
  return NULL;
}

TypeCode * OptimizerAlgBase::getTCForInProxy() const
{
  return getTCForIn();
}

TypeCode * OptimizerAlgBase::getTCForOutProxy() const
{
  return getTCForOut();
}

TypeCode * OptimizerAlgBase::getTCForAlgoInitProxy() const
{
  return getTCForAlgoInit();
}

TypeCode * OptimizerAlgBase::getTCForAlgoResultProxy() const
{
  return getTCForAlgoResult();
}

void OptimizerAlgBase::initializeProxy(const Any *input) throw (YACS::Exception)
{
  initialize(input);
}

void OptimizerAlgBase::startProxy()
{
  start();
}

void OptimizerAlgBase::takeDecisionProxy()
{
  takeDecision();
}

void OptimizerAlgBase::finishProxy()
{
  finish();
  _errorMessage = "";
}

Any * OptimizerAlgBase::getAlgoResultProxy()
{
  return getAlgoResult();
}

void OptimizerAlgBase::setPool(Pool* pool)
{
  _pool=pool;
}

void OptimizerAlgBase::setProc(Proc * proc)
{
  _proc = proc;
}

Proc * OptimizerAlgBase::getProc()
{
  return _proc;
}

bool OptimizerAlgBase::hasError() const
{
  return (_errorMessage.length() > 0);
}

const std::string & OptimizerAlgBase::getError() const
{
  return _errorMessage;
}

void OptimizerAlgBase::setError(const std::string & message)
{
  _errorMessage = (message.length() > 0) ? message : "Unknown error";
}

void OptimizerAlgBase::setNbOfBranches(int nbOfBranches)
{
  _nbOfBranches=nbOfBranches;
}

int OptimizerAlgBase::getNbOfBranches() const
{
  return _nbOfBranches;
}

OptimizerAlgASync::OptimizerAlgASync(Pool *pool):OptimizerAlgBase(pool)
{
}

OptimizerAlgASync::~OptimizerAlgASync()
{
}

void OptimizerAlgASync::finishProxy()
{
  terminateSlaveThread();
  OptimizerAlgBase::finishProxy();
}

void OptimizerAlgASync::takeDecision()
{
  signalSlaveAndWait();
}

void OptimizerAlgASync::start()
{
  AlternateThread::start();
}

void OptimizerAlgASync::run()
{
  startToTakeDecision();
  _pool->destroyAll();
}
