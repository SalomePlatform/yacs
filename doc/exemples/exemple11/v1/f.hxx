
extern "C" float f1(float *);
extern "C" float f2(float *);
extern "C" void setcom(float *);
extern "C" void getcom(float *);

class f {

public:

  f() {
    float y = 1.0;
    setcom(&y);
  }

  void setCommon(float x) {
    setcom(&x);
  }

  float getCommon() {
    float x;
    getcom(&x);
    return x;
  }

  float f1(float x) {
    return ::f1(&x);
  }

  float f2(float x) {
    return ::f2(&x);
  }
};
