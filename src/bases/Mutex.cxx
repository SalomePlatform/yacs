#include "Mutex.hxx"

#if defined(YACS_PTHREAD)
#include "MutexPT.cxx"
#else
#error
#endif
