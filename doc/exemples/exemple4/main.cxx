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

#include "FreeFem.hxx"

int main()
{
  FreeFem C;

  vector<sBord> B(1);
  B[0].X = "x=cos(2*pi*t)";
  B[0].Y = "y=sin(2*pi*t)";
  B[0].n = 100;

  C.Bords(B);
  C.Flux("y", "-x");
  C.Convection("exp(-10*((x-0.3)^2 +(y-0.3)^2))", 0.1, 40);
}
