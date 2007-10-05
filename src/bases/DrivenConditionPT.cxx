#include "DrivenConditionPT.hxx"

using namespace YACS::BASES;

DrivenConditionPT::DrivenConditionPT()
{
  pthread_mutex_init(&_mutexDesc1, NULL);
  pthread_mutex_init(&_mutexDesc2, NULL);
  pthread_cond_init(&_cond1, NULL);
  pthread_cond_init(&_cond2, NULL);
  pthread_mutex_lock(&_mutexDesc1);
  pthread_mutex_lock(&_mutexDesc2);
}

DrivenConditionPT::~DrivenConditionPT()
{
  pthread_cond_destroy(&_cond1);
  pthread_cond_destroy(&_cond2);
  pthread_mutex_destroy(&_mutexDesc1);
  pthread_mutex_destroy(&_mutexDesc2);
}

void DrivenConditionPT::notifyOneSync()
{
  pthread_mutex_lock(&_mutexDesc1);
  pthread_cond_signal(&_cond1);
  pthread_mutex_unlock(&_mutexDesc1);
  pthread_cond_wait(&_cond2, &_mutexDesc2);
}

void DrivenConditionPT::waitForAWait()
{
  pthread_cond_wait(&_cond2, &_mutexDesc2);
}

void DrivenConditionPT::wait()
{
  pthread_mutex_lock(&_mutexDesc2);
  pthread_cond_signal(&_cond2);
  pthread_mutex_unlock(&_mutexDesc2);
  pthread_cond_wait(&_cond1,&_mutexDesc1);
}
