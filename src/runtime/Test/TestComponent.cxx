
#include "Any.hxx"
#include <iostream>
#include <cstring>
#include <cmath>
#include "TestComponent.hxx"


extern "C" void * __init()
{
	TestComponent * obj = new TestComponent();
	obj->info.code = 0;
	obj->info.message = "";
	return obj;
}
 
extern "C" void __terminate(void **pObj)
{
	TestComponent * obj = * (TestComponent **) pObj;
	delete obj;
	*pObj = NULL;
}

extern "C" void __ping()
{
	std::cerr << "ping TestComponent" << std::endl;
}
    
extern "C" void __run(void *vObj, const char *service, int nIn, int nOut, 
		         YACS::ENGINE::Any **In, YACS::ENGINE::Any **Out, returnInfo * r)
{
	TestComponent * obj = (TestComponent *) vObj;
	
	if (obj == NULL) {
		r->code = -1;
		r->message = "TestComponent has not been initialized";
		return;
	}
	
	obj->info.message = "";
	obj->info.code = 0;
	
	if (std::strncmp(service, "f", 1) == 0) 
	{
		double _arg0 = In[0]->getDoubleValue();
		double _res = obj->f(_arg0);
		Out[0] = YACS::ENGINE::AtomAny::New(_res);
	}
	else
	{
		obj->info.code = 1;
		obj->info.message = "service ";
		obj->info.message += service;
		obj->info.message += " doesn't exist in TestComponent";
		Out[0] = NULL;
	}
	*r = obj->info;
}

double TestComponent::f(double x)
{
	double y;
	
	if (x >= 0.0)
	{
	  y = std::sqrt(x);
	}
	else {
	  y = 0.0;
	  info.message = "TestComponent::f : argument must be positive or null";
	  info.code = 2;
	}
	return y;
}

double TestComponent::g(int n, double x)
{
	double y;
	y = std::pow(x, n);
	return y;
}
