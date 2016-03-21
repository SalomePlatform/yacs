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

#ifndef __DYNLIBLOADER_HXX__
#define __DYNLIBLOADER_HXX__

#include "YACSBasesExport.hxx"

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
#if defined(WIN32)
#include "DynLibLoaderWin.hxx"
#else
#include "DynLibLoaderGNU.hxx"
#endif

namespace YACS
{
  namespace BASES
  {
#if defined(WIN32)
    typedef DynLibLoaderWin DynLibLoader;
#else
    typedef DynLibLoaderGNU DynLibLoader;
#endif
  }
}

#else

#error

#endif

#endif
