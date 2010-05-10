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
