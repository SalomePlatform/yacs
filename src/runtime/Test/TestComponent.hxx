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
