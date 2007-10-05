#include "MutexPT.hxx"

using namespace YACS::BASES;

MutexPT::MutexPT()
{
  //pthread_mutexattr_settype(&_options, PTHREAD_MUTEX_FAST_NP);
  //pthread_mutex_init(&_mutexDesc, &_options);
  pthread_mutex_init(&_mutexDesc, NULL); // potential hang up at start with commented init
}

MutexPT::~MutexPT()
{
  pthread_mutex_destroy(&_mutexDesc);
}

void MutexPT::lock()
{
  pthread_mutex_lock(&_mutexDesc);
}

void MutexPT::unlock()
{
  pthread_mutex_unlock(&_mutexDesc);
}

ConditionPT::ConditionPT()
{
  pthread_cond_init(&_cond,  NULL);
}

ConditionPT::~ConditionPT()
{
  pthread_cond_destroy(&_cond);
}

void ConditionPT::notify_one()
{
  pthread_cond_signal(&_cond);
}

void ConditionPT::notify_all()
{
  pthread_cond_broadcast(&_cond);
}

void ConditionPT::wait(MutexPT& mutex)
{
  pthread_cond_wait(&_cond, &mutex._mutexDesc);
}

