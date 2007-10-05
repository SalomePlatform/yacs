#ifndef __DYNLIBLOADERGNU_HXX__
#define __DYNLIBLOADERGNU_HXX__

#include <string>

namespace YACS
{
  namespace BASES
  {
    class DynLibLoaderGNU
    {
    private:
      void *_handleOnLoadedLib;
      std::string _libName;
      static const char _extForDynLib[];
    public:
      DynLibLoaderGNU(const std::string& libNameWithoutExtension);
      ~DynLibLoaderGNU();
      bool isLibFileFindable() const;
      std::string getLibNameWithoutExt() const;
      int appendDirInSearchPath(const std::string& dirName);
      int removeDirInSearchPath(const std::string& dirName);
      void *getHandleOnSymbolWithName(const std::string& symbName, bool stopOnError=true);
      static const char *getExtensionForDynLib();
      bool load();
      bool reload();
      bool unload();

    private:
      void loadLib() { load(); }//! load lib without regarding that _libName is reachable
      void *resolveSymb(const std::string& symbName, bool stopOnError);
    };
  }
}

#endif
