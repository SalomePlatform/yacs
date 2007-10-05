#ifndef __DYNLIBLOADERWIN_HXX__
#define __DYNLIBLOADERWIN_HXX__

#include <string>

namespace YACS
{
  namespace BASES
  {
    class DynLibLoaderWin
    {
    private:
      void *_handleOnLoadedLib;
      std::string _libName;
      static const char _extForDynLib[];
    public:
      DynLibLoaderWin(const std::string& libNameWithoutExtension);
      ~DynLibLoaderWin();
      bool isLibFileFindable() const;
      int appendDirInSearchPath(const std::string& dirName);
      int removeDirInSearchPath(const std::string& dirName);
      void *getHandleOnSymbolWithName(const std::string& symbName);
      static const char *getExtensionForDynLib();
    private:
      void loadLib();//! load lib without regarding that _libName is reachable
      void *resolveSymb(const std::string& symbName);
    };
  }
}

#endif
