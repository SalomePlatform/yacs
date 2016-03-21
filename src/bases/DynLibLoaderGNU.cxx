// Copyright (C) 2006-2016  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include "DynLibLoaderGNU.hxx"
#include <iostream>
#include <dlfcn.h>
#include "Exception.hxx"

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
  dlerror();
  _handleOnLoadedLib=dlopen(fullLibName.c_str(),RTLD_LAZY | RTLD_GLOBAL);
  char *message=dlerror();
  if (message != NULL)
    {
      std::string error = "Error while trying to load library with name " + fullLibName +
                          " with the following internal message: " + message;
      throw YACS::Exception(error);
    }
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
      std::string error="Error detected on symbol ";
      error=error+symbName +" search in library with name "+_libName+_extForDynLib+" with the following internal message "+message;
      throw YACS::Exception(error);
    }
  else
    return ret;
}

const char *DynLibLoaderGNU::getExtensionForDynLib()
{
  return _extForDynLib+1;
}

