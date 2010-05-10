class vecteur {

public:
  vecteur(long n);
  ~vecteur();

  double * x() { return xx; };
  long     n() { return nn; };

private:
  double *xx;
  long nn;
};
