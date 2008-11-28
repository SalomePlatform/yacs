//  Copyright (C) 2006-2008  CEA/DEN, EDF R&D
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
#include "DynLibLoaderWin.hxx"
#include <iostream>
#include <Windows.h>

using namespace YACS::BASES;

const char DynLibLoaderWin::_extForDynLib[]=".dll";

DynLibLoaderWin::DynLibLoaderWin(const std::string& libNameWithoutExtension):_libName(libNameWithoutExtension),
                                                                             _handleOnLoadedLib(0)
{
}

DynLibLoaderWin::~DynLibLoaderWin()
{
  if(_handleOnLoadedLib)
    dlclose(_handleOnLoadedLib);
}

bool DynLibLoaderWin::isLibFileFindable() const
{
  return true;
}

/*!
  Append a directory with name \b dirName to the searching paths.
  \return If append succeeds 0 is returned.
  If the directory does not exists 1 is returned.
  If the addition of directory causes some troubles due to seach paths name 2 is returned.
 */
int DynLibLoaderWin::appendDirInSearchPath(const std::string& dirName)
{
  return 0;
}

/*!
  Removes a directory with name \b dirName from the searching paths.
  \return If removal succeeds 0 is returned.
  If the directory does not exists 1 is returned.
  If the path were not already in existing paths 2 is returned.
 */
int DynLibLoaderWin::removeDirInSearchPath(const std::string& dirName)
{
  return 0;
}

void *DynLibLoaderWin::getHandleOnSymbolWithName(const std::string& symbName)
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
  return resolveSymb(symbName);
}

void DynLibLoaderWin::loadLib()
{
  std::string fullLibName(_libName);
  fullLibName+=_extForDynLib;
  _handleOnLoadedLib=LoadLibrary(fullLibName.c_str());
}

void *DynLibLoaderWin::resolveSymb(const std::string& symbName)
{
  void *ret=GetProcAddress(_handleOnLoadedLib,symbName.c_str());
  return ret;
}

const char *DynLibLoaderWin::getExtensionForDynLib()
{
  return _extForDynLib+1;
}

