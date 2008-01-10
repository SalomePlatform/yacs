#ifndef __yacsconfig_h__
#define __yacsconfig_h__

#include <yacs_config.h>

#undef PACKAGE_BUGREPORT
#undef PACKAGE_NAME
#undef PACKAGE_STRING
#undef PACKAGE_TARNAME
#undef PACKAGE_VERSION

#if defined __GNUC__ 
  #if (__GNUC__ == 3 && __GNUC_MINOR__ < 4)
    #define NOCOVARIANT
  #endif
#endif

#endif

