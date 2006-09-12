#include "SemaphorePT.hxx"

using namespace YACS::BASES;

SemaphorePT::SemaphorePT(int initValue)
{
  sem_init(&_semDesc, 0, initValue);
}

SemaphorePT::~SemaphorePT()
{
  sem_destroy(&_semDesc);
}

void SemaphorePT::post()
{
  sem_post(&_semDesc);
}

void SemaphorePT::wait()
{
  sem_wait(&_semDesc);
}

int SemaphorePT::getValue()
{
  int val;
  sem_getvalue(&_semDesc,&val);
  return val;
}
