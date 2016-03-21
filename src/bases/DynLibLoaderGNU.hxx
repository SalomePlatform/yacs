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

      // Forbid the use of copy constructor and assignment operator to avoid messing up
      // with the library handle
      DynLibLoaderGNU(const DynLibLoaderGNU & orig) {}
      DynLibLoaderGNU& operator=(const DynLibLoaderGNU & orig) { return *this; }
    };
  }
}

#endif
