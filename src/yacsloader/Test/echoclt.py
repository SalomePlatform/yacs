#!/usr/bin/env python
# Copyright (C) 2006-2016  CEA/DEN, EDF R&D
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

import sys

# Import the CORBA module
from omniORB import CORBA
import CosNaming

import eo

# Initialise the ORB
orb = CORBA.ORB_init(sys.argv, CORBA.ORB_ID)

# Obtain a reference to the root naming context
obj         = orb.resolve_initial_references("NameService")
rootContext = obj._narrow(CosNaming.NamingContext)

if rootContext is None:
    print "Failed to narrow the root naming context"
    sys.exit(1)

# Resolve the name "test.my_context/Echo.Object"
name = [CosNaming.NameComponent("test", "my_context"),
        CosNaming.NameComponent("Echo", "Object")]

try:
    obj = rootContext.resolve(name)

except CosNaming.NamingContext.NotFound, ex:
    print "Name not found"
    sys.exit(1)

# Narrow the object to an eo::Echo
echo = obj._narrow(eo.Echo)

if echo is None:
    print "Object reference is not an eo::Echo"
    sys.exit(1)

# Invoke the echoString operation
message = "Hello from Python"
result  = echo.echoString(message)

print "I said '%s'. The object said '%s'." % (message,result)

"""
  struct S1
  {
    double x;
    long y;
    string s;
    boolean b;
    DoubleVec vd;
  };
  struct S2
  {
    S1 s;
  };
"""

s1=eo.S1(x=1,y=2,s="aa",b=True,vd=[1,2])
s2=eo.S2(s1)

r=echo.echoStruct(s2)
print r

s3=eo.S3(x=1,y=2,s="aa",b=True,ob=None)
r=echo.echoStruct2(s3)
print r

ob=echo.createObj(3)
print ob
oc=echo.createC()
print oc

s3=eo.S3(x=1,y=2,s="aa",b=True,ob=ob)
r=echo.echoStruct2(s3)
print r

s3=eo.S3(x=1,y=2,s="aa",b=True,ob=oc)
r=echo.echoStruct2(s3)
print r

r=echo.echoObjectVec([ob,ob])
print r

r=echo.echoObjectVec([oc,oc])
print r

r=echo.echoObjectVec([ob,oc])
print r

#echo.shutdown()
