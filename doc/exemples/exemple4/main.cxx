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
