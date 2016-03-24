// Copyright (C) 2006-2016  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

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
	
        Out[0] = NULL;
	if (std::strncmp(service, "f", 1) == 0) 
	{
		double _arg0 = In[0]->getDoubleValue();
		double _res = obj->f(_arg0);
		if (obj->info.code == 0)
		  Out[0] = YACS::ENGINE::AtomAny::New(_res);
	}
	else
	{
		obj->info.code = 1;
		obj->info.message = "service ";
		obj->info.message += service;
		obj->info.message += " doesn't exist in TestComponent";
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
