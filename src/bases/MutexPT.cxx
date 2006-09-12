#include "MutexPT.hxx"

using namespace YACS::BASES;

MutexPT::MutexPT()
{
  pthread_mutexattr_settype(&_options, PTHREAD_MUTEX_FAST_NP);
  pthread_mutex_init(&_mutexDesc, &_options);
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
