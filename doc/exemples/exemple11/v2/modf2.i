%module modf2

%{
#include "f2.hxx"
%}

class f {

public:

  f();
  float f2(float x);
  void setCommon(float x);
  float getCommon();
};
