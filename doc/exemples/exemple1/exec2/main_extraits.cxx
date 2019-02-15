  alglin CompInterne;

  long n = 5;
  vecteur *A = CompInterne.create_vector(n);
  vecteur *B = CompInterne.create_vector(n);
  vecteur *C = CompInterne.create_vector(n);

// ...

  CompInterne.addvec(C, A, B);
  cout << CompInterne.prdscl(A, B) << endl;

