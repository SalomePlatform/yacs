#include <string>
#include <vector>

struct sBord {
  string X;
  string Y;
  int n;
};

class FreeFem
{
 public:

  void Bords(vector<sBord> &B);
  void Flux(string u1, string u2);
  void Convection(string Cinit, double dt, long n);

 private:

  string VX_, VY_;
  vector<sBord> B_;
};
