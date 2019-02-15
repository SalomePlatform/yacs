%module modf1

%{
#include "f1.hxx"
%}

class f {

public:

  f();
  float f1(float x);
  void setCommon(float x);
  float getCommon();
};
