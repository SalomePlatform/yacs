%module AlgLinModule

%{
#include "alglin.hxx"
#include <string>
static string tampon;

%}

class vecteur {
 
public:
  vecteur(long n);
  ~vecteur();
 
  double * x();
  long     n();
%addmethods {
  double __getitem__(long i) 
    { double *x_ = self->x(); return x_[i]; }
  double __setitem__(long i, double value) 
    { double *x_ = self->x(); x_[i] = value; }
  const char *   __str__() {
     tampon = "";
     double *x_ = self->x();
     long n_ = self->n();
     char sx[20];
     for (long i=0; i<n_; i++) {
         sprintf(sx, " %10.4g", x_[i]);
         tampon += sx;
         }
     return tampon.c_str();
     }
  }
};

class alglin {

public:
  alglin();
  ~alglin();
  void      addvec(vecteur *C, vecteur *A, vecteur *B);
  double    prdscl(vecteur *A, vecteur *B);
  vecteur * create_vector(long n);
  void      destroy_vector(vecteur *V);

};

