#include "ContainerTest.hxx"
#include "ComponentInstance.hxx"
#include "ToyNode.hxx"
#include <sstream>

using namespace std;
using namespace YACS::ENGINE;

unsigned ContainerTest::_counter = 0;

const char ContainerTest::SUPPORTED_COMP_KIND[] = "TESTKIND1";

unsigned ContainerTest2::_counter = 0;

const char ContainerTest2::SUPPORTED_COMP_KIND[] = "TESTKIND2";

ContainerTest::ContainerTest():_alreadyStarted(false),_myCounter(_counter++)
{
}

std::string ContainerTest::getPlacementInfo() const
{
  ostringstream stream;
  stream << "Cont#_" << _myCounter;
  return stream.str();
}

bool ContainerTest::isAlreadyStarted() const
{
  return _alreadyStarted;
}

void ContainerTest::start() throw(Exception)
{
  if(_alreadyStarted)
    throw Exception("ContainerTest already started !!!!");
  _alreadyStarted=true;
}

Container *ContainerTest::clone() const
{
  if(_isAttachedOnCloning)
    {
      incrRef();
      return (Container *) this;
    }
  else
    return new ContainerTest;
}

void ContainerTest::checkCapabilityToDealWith(const ComponentInstance *inst) const throw(Exception)
{
  if(inst->getKind()!=SUPPORTED_COMP_KIND)
    throw Exception("ContainerTest not compatible with this type of instance.");
}

void ContainerTest::initAllContainers()
{
  _counter=0;
}

ContainerTest2::ContainerTest2():_alreadyStarted(false),_myCounter(_counter++)
{
}

bool ContainerTest2::isAlreadyStarted() const
{
  return _alreadyStarted;
}

void ContainerTest2::start() throw(Exception)
{
  if(_alreadyStarted)
    throw Exception("ContainerTest already started !!!!");
  _alreadyStarted=true;
}

Container *ContainerTest2::clone() const
{
  if(_isAttachedOnCloning)
    {
      incrRef();
      return (Container *) this;
    }
  else
    return new ContainerTest2;
}

void ContainerTest2::initAllContainers()
{
  _counter=0;
}

void ContainerTest2::checkCapabilityToDealWith(const ComponentInstance *inst) const throw(Exception)
{
  if(inst->getKind()!=SUPPORTED_COMP_KIND)
    throw Exception("ContainerTest not compatible with this type of instance.");
}
