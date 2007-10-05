#include "DynLibLoader.hxx"

#if defined(YACS_PTHREAD)
#include "DynLibLoaderGNU.cxx"
#else
#error
#endif
