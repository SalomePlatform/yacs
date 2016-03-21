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

#include "Pool.hxx"
#include "Any.hxx"

#include <sstream>
#include <set>

using namespace YACS::ENGINE;

Any *Pool::ExpData::NOT_USED_NOR_COMPUTED = 0;

Any *Pool::ExpData::USED_BUT_NOT_COMPUTED_YET = (Any *) 1;

const char Pool::MESSAGEFORUNXSTNGID[]="The id specified not exists. Unable to handle with. Either internal error, or invalid use of Pool from Optimizer Algorithm";

Pool::ExpData::ExpData(Any *inValue, unsigned char priority):_in(inValue),_out(NOT_USED_NOR_COMPUTED),_priority(priority)
{
  if(_in)
    _in->incrRef();
}

Pool::ExpData::ExpData(const ExpData& other):_in(other._in),_out(other._out),_priority(other._priority)
{
  if(_in)
    _in->incrRef();
}

Pool::ExpData::~ExpData()
{
  if(_in)
    _in->decrRef();
  if(_out!=USED_BUT_NOT_COMPUTED_YET && _out!=NOT_USED_NOR_COMPUTED)
    _out->decrRef();
}

/*!
 * When used _out is assumed to be equal to 'USED_BUT_NOT_COMPUTED_YET' before call.
 */
void Pool::ExpData::setOutValue(Any *outValue)
{
  if(_out!=USED_BUT_NOT_COMPUTED_YET && _out!=NOT_USED_NOR_COMPUTED)
    _out->decrRef();//should absolutely never happend.
  _out=outValue;
  _out->incrRef();
}

/*!
 * When called _out is assumed to be equal to 'NOT_USED_NOR_COMPUTED' before call.
 */
void Pool::ExpData::markItAsInUse()
{
  _out=USED_BUT_NOT_COMPUTED_YET;
}

bool Pool::ExpData::isLaunchable() const
{
  return _out==NOT_USED_NOR_COMPUTED;
}

int Pool::getCurrentId() const 
{ 
  if(empty())
    throw YACS::Exception("no current case set in pool");
  else
    return _currentCase->first; 
}
Any *Pool::getCurrentInSample() const 
{ 
  if(empty())
    throw YACS::Exception("no current case set in pool");
  else
    return (*_currentCase).second.inValue(); 
}

Any *Pool::getCurrentOutSample() const 
{ 
  if(empty())
    throw YACS::Exception("no current case set in pool");
  else
    return (*_currentCase).second.outValue(); 
}

Any *Pool::getOutSample(int id)
{
  if(empty())
    throw YACS::Exception("no current case set in pool");

  std::list< std::pair<int, ExpData> >::iterator iter;
  for(iter=_container.begin();iter!=_container.end();iter++)
    if((*iter).first==id)
        return (*iter).second.outValue();
  if(iter==_container.end())
    throw YACS::Exception("no current case set in pool");
}


//! Push a sample. \b WARNING inSample ownership is released to current Pool instance (this) !
void Pool::pushInSample(int id, Any *inSample, unsigned char priority)
{
  std::pair<int, ExpData> eltToAdd(id,Pool::ExpData(inSample,priority));
  _container.push_back(eltToAdd);
  inSample->decrRef();
}

void Pool::destroyAll()
{
  _container.clear();
}

void Pool::destroyCurrentCase()
{
  if(!_container.empty())
    _container.erase(_currentCase);
}

/*!
 *
 * This method is typically called by OptimizerNode to check the consistency, that is to say that optimizer algorithm has not
 * corrupted 'this'.
 *
 */
void Pool::checkConsistency() throw(YACS::Exception)
{
  // First check unicity of ids.
  std::set<int> ids;
  std::list< std::pair<int, ExpData> >::iterator iter;
  for(iter=_container.begin();iter!=_container.end();iter++)
    {
      std::pair< std::set<int>::iterator, bool > verdict=ids.insert((*iter).first);
      if(verdict.second)
        {
          std::ostringstream what;
          what << "Id with value : " << (*iter).first << " appears several times.";
          throw Exception(what.str());
        }
    }
}

/*!
 * \throw See the \b throw case of pushOutSampleAt method.
 */
void Pool::setCurrentId(int id) throw(YACS::Exception)
{
  std::list< std::pair<int, ExpData> >::iterator iter;
  for(iter=_container.begin();iter!=_container.end();iter++)
    if((*iter).first==id)
      {
        _currentCase=iter;
        break;
      }
  if(iter==_container.end())
    throw Exception(MESSAGEFORUNXSTNGID);
}

/*!
 *
 * Push a result of case discriminated by \b id. It also sets the \b _currentCase pointer on the case discriminated by \b id.
 * So after this call, the call to setCurrentId with the same \b id is useless.
 * \throw When case id is not found in 'this'. This is particulary true, if not an internal error, when optimizer algorithm  
 *        has destroyed a case id different from its id.
 *
 */
void Pool::putOutSampleAt(int id, Any *outValue) throw(YACS::Exception)
{
  std::list< std::pair<int, ExpData> >::iterator iter;
  for(iter=_container.begin();iter!=_container.end();iter++)
    if((*iter).first==id)
      {
        _currentCase=iter;
        (*iter).second.setOutValue(outValue);
        break;
      }
  if(iter==_container.end())
    throw Exception(MESSAGEFORUNXSTNGID);
}

/*!
 *
 * This method is typically called by OptimizerNode instance owner of 'this' that wants to launch an another job on one branch.
 * \return : In case there are more jobs to do 2 parameters are returned.
 *           - \b id to locate the computation to do.
 *           - \b priority attached.
 *           - \b value.
 *           In case no more jobs are required id and priority stay unchanged and the returned value is equal to 0.
 *
 */
Any *Pool::getNextSampleWithHighestPriority(int& id, unsigned char& priority) const
{
  unsigned char myPriority=0;
  std::list< std::pair<int, ExpData> >::const_iterator iter,ptToSelected;
  ptToSelected=_container.end();
  for(iter=_container.begin();iter!=_container.end();iter++)
    {
      if((*iter).second.isLaunchable())
        if((*iter).second.getPriority()>myPriority || ptToSelected==_container.end())
          {
            ptToSelected=iter;
            myPriority=(*iter).second.getPriority();
          }
    }
  //Search performed. No performing output writings if needed.
  if(ptToSelected==_container.end())
    return 0;
  priority=myPriority;
  id=(*ptToSelected).first;  
  return (*ptToSelected).second.inValue();
}

/*!
 *
 * Typically called after 'this->destroyCurrentCase' 'this->checkConsistency' and 'this->getNextSampleWithHighestPriority' have been called.
 * At this point the case with id \b id is marked as in use in order to avoid to be used by an another branch of OptimizerNode.
 *
 */
void Pool::markIdAsInUse(int id)
{
  std::list< std::pair<int, ExpData> >::iterator iter;
  for(iter=_container.begin();iter!=_container.end();iter++)
    if((*iter).first==id)
      {
        (*iter).second.markItAsInUse();
        break;
      }
}

/*!
 * Typically called after takeDecision of OptimizerAlg as been performed. If true is returned, that is to say that convergence has been reached.
 */
bool Pool::empty() const
{
  return _container.empty();
}
