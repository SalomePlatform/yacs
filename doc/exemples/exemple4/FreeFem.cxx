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
#include <fstream>

void FreeFem::Bords(vector<sBord> &B) {
  B_ = B;
}

void FreeFem::Flux(string u1, string u2) {
  VX_ = u1;
  VY_ = u2;
}

void FreeFem::Convection(string Cinit, double dt, long n) {

  ofstream f("/tmp/example.edp");
  
  int i, nB = B_.size();

  for (i=0; i<nB; i++)
    f << "border b" << i << "(t=0,1){" 
      << B_[i].X << "; " << B_[i].Y << "; }" << endl;

  f << "mesh th = buildmesh(";
  for (i=0; i<nB; i++)
    f << "b" << i << "(" << B_[i].n << ")" 
      << ( i<nB-1 ? '+' : ')');
  f << ";" << endl;

  f << "fespace Vh(th,P1);" << endl;
  f << "Vh v = " << Cinit << ";" << endl << "plot(v);" <<endl;
  f << "real dt = " << dt << ", t=0;" << endl;
  f << "Vh u1 = " << VX_ << ", u2 = " << VY_ << ";" << endl;
  f << "int i;" << endl << "Vh vv,vo;" << endl
    << "for ( i=0; i<" << n << "; i++) {" << endl
    << "t += dt;" << endl << "vo=v;" << endl
    << "v=convect([u1,u2],-dt,vo);" << endl
    << "plot(v,wait=0);" << endl << "};" << endl;
 
  f.close();

  system("FreeFem++ /tmp/example.edp");
}
