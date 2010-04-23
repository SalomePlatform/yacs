//  Copyright (C) 2006-2010  CEA/DEN, EDF R&D
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

extern "C" float f1(float *);
extern "C" void setcom(float *);
extern "C" void getcom(float *);

class f {

public:

  f() {
    float y = 1.0;
    setcom(&y);
  }
  void setCommon(float x) {
    setcom(&x);
  }
  float getCommon() {
    float x;
    getcom(&x);
    return x;
  }
  float f1(float x) {
    return ::f1(&x);
  }
};
