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

#include <CORBA.h>
#include <fstream>
#include <string>
#include "alglin.hh"

int main(int argc,char **argv)
{
  CORBA::ORB_ptr orb = CORBA::ORB_init (argc, argv);

  string s;
  ifstream f("AlgLin.ior");
  f >> s;
  f.close();

  CORBA::Object_ptr O = orb->string_to_object(s.c_str());
  Distant::AlgLin_var A = Distant::AlgLin::_narrow(O);

  long n = 5;

  Distant::vecteur V1;
  V1.length(n);

  Distant::vecteur V2;
  V2.length(n);

  for (long i=0; i<n; i++) {
    V1[i] = i*i;
    V2[i] = 2*i;
  }

  CORBA::Double S = A->prdscl(V1, V2);
  cerr << S << endl;
}
