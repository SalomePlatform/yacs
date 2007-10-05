#ifndef __DYNLIBLOADER_HXX__
#define __DYNLIBLOADER_HXX__

#include <string>

// --- Interface is:

// class DynLibLoader
// {
// public:
//   DynLibLoader(const std::string& libNameWithoutExtension);
//   ~DynLibLoader();
//   bool isLibFileFindable() const;
//   int appendDirInSearchPath(const std::string& dirName);
//   int removeDirInSearchPath(const std::string& dirName);
//   void *getHandleOnSymbolWithName(const std::string& symbName);
//   static const char *getExtensionForDynLib();
// };


#if defined(YACS_PTHREAD)
#include "DynLibLoaderGNU.hxx"

namespace YACS
{
  namespace BASES
  {
    typedef DynLibLoaderGNU DynLibLoader;
  }
}

#else

#error

#endif

#endif
