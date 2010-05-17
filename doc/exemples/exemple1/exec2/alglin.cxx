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

#include "alglin.hxx"

extern "C" {
  double * valloc(long n);
  void vfree(double *x);
  void addvec_(double *C, double *A, double *B, long *n);
  double prdscl_(double *A, double *B, long *n);
}

void   alglin::addvec(vecteur *C, vecteur *A, vecteur *B) {
  long n = A->n();
  addvec_(C->x(), A->x(), B->x(), &n);
}

double alglin::prdscl(vecteur *A, vecteur *B) {
  long n = A->n();
  return prdscl_(A->x(), B->x(), &n);
}

vecteur * alglin::create_vector(long n) {
  return new vecteur(n);
}

void   alglin::destroy_vector(vecteur *V) {
  delete V;
}
