#ifndef TESTCOMPONENT_HXX_
#define TESTCOMPONENT_HXX_


#include "Any.hxx"
#include <exception>

struct returnInfo {
	int code;
	std::string message;
};

class TestComponent {

public :
	
	double f(double c);
	double g(int n, double x);
	returnInfo info;
};

extern "C" void * __init();
extern "C" void __terminate(void **pObj);
extern "C" void __ping();
extern "C" void __run(void *vObj, const char *service, int nIn, int nOut, 
		              YACS::ENGINE::Any **In, YACS::ENGINE::Any **Out, returnInfo *r);

#endif /*TESTCOMPONENT_HXX_*/
