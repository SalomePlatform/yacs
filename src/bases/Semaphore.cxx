#include "Semaphore.hxx"

#if defined(YACS_PTHREAD)
#include "SemaphorePT.cxx"
#else
#error
#endif
