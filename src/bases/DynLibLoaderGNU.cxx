#include "DynLibLoaderGNU.hxx"
#include <iostream>
#include <dlfcn.h>

using namespace YACS::BASES;

const char DynLibLoaderGNU::_extForDynLib[]=".so";

DynLibLoaderGNU::DynLibLoaderGNU(const std::string& libNameWithoutExtension):_libName(libNameWithoutExtension),
                                                                             _handleOnLoadedLib(0)
{
}

DynLibLoaderGNU::~DynLibLoaderGNU()
{
  if(_handleOnLoadedLib)
    dlclose(_handleOnLoadedLib);
}

bool DynLibLoaderGNU::isLibFileFindable() const
{
  return true;
}

std::string DynLibLoaderGNU::getLibNameWithoutExt() const
{
  return _libName;
}

/*!
  Append a directory with name \b dirName to the searching paths.
  \return If append succeeds 0 is returned.
  If the directory does not exists 1 is returned.
  If the addition of directory causes some troubles due to seach paths name 2 is returned.
 */
int DynLibLoaderGNU::appendDirInSearchPath(const std::string& dirName)
{
  return 0;
}

/*!
  Removes a directory with name \b dirName from the searching paths.
  \return If removal succeeds 0 is returned.
  If the directory does not exists 1 is returned.
  If the path were not already in existing paths 2 is returned.
 */
int DynLibLoaderGNU::removeDirInSearchPath(const std::string& dirName)
{
  return 0;
}

void *DynLibLoaderGNU::getHandleOnSymbolWithName(const std::string& symbName, bool stopOnError)
{
  if(!_handleOnLoadedLib)
    if(!isLibFileFindable())
      {
        std::cerr << "Dynamic library with name " << symbName << _extForDynLib;
        std::cerr << " not existing in paths specified" << std::endl;
        return 0;
      }
    else
      loadLib();
  return resolveSymb(symbName, stopOnError);
}

bool DynLibLoaderGNU::load()
{
  std::string fullLibName(_libName);
  fullLibName+=_extForDynLib;
  _handleOnLoadedLib=dlopen(fullLibName.c_str(),RTLD_LAZY | RTLD_GLOBAL);
  return _handleOnLoadedLib != NULL;
}

bool DynLibLoaderGNU::unload()
{
  if (_handleOnLoadedLib) 
    {
      dlclose(_handleOnLoadedLib);
      _handleOnLoadedLib = NULL;
    }
}

bool DynLibLoaderGNU::reload()
{
  unload();
  return load();
}

void *DynLibLoaderGNU::resolveSymb(const std::string& symbName, bool stopOnError)
{
  dlerror();
  void *ret=dlsym(_handleOnLoadedLib,symbName.c_str());
  char *message=dlerror();
  if(stopOnError && (NULL != message))
    {
      std::cerr << "Error detected on symbol " << symbName << " search in library with name " << _libName << _extForDynLib;
      std::cerr << " with the following internal message"<<  std::endl; 
      std::cerr << message << std::endl;
      return 0;
    }
  else
    return ret;
}

const char *DynLibLoaderGNU::getExtensionForDynLib()
{
  return _extForDynLib+1;
}

