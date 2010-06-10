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
