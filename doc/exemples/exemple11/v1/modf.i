%module modf

%{
#include "f.hxx"
%}

class f {

public:

  void setCommon(float x);
  float getCommon();
  float f1(float x);
  float f2(float x);
};
