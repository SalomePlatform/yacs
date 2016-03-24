// Copyright (C) 2007-2016  CEA/DEN, EDF R&D
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
// Author : InckA

%include exception.i 

%{
#include "PMMLlib.hxx"
%}

%exception { 
    try {
        $action
    } catch (std::string &e) {
        std::string s("PMMLlib error: "), s2(e);
        s = s + s2;
        SWIG_exception(SWIG_RuntimeError, s.c_str());
    } catch (...) {
        SWIG_exception(SWIG_RuntimeError, "unknown exception");
    }
}

namespace PMMLlib
{

enum PMMLType{kUNDEFINED, kANN, kLR};

class PMMLlib
{
  public:
    PMMLlib(bool log=false);
    PMMLlib(std::string file, bool log=false);
    ~PMMLlib();
    void SetCurrentModel(std::string modelName, PMMLType type);
    std::string ExportPyStr(std::string functionName, std::string header); 
    void ExportPython(std::string file, std::string functionName, std::string header);
    void ExportCpp(std::string file, std::string functionName, std::string header);
    void Write();
    void Write(std::string file);
};

}



