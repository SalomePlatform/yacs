#!/usr/bin/env python3
# Copyright (C) 2006-2024  CEA, EDF
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

import xmlrpc.client,sys

data="""
<methodCall>
  <methodName>echo</methodName>
  <params>
    <param><value>hello, world</value></param>
    <param><value><double>3.5</double></value></param>
    <param><value><string>coucou</string></value></param>
  </params>
</methodCall>
"""
def echo(args):
  print(args)
  return 96785439

f=open("input")
data=f.read()
f.close()

params, method = xmlrpc.client.loads(data)

try:
   call=eval(method)
   response=call(params)
   response = (response,)
except:
   # report exception back to server
   response = xmlrpc.client.dumps( xmlrpc.client.Fault(1, "%s:%s" % sys.exc_info()[:2]))
else:
   response = xmlrpc.client.dumps( response, methodresponse=1)

print(response)
f=open("output",'w')
f.write(response)
f.close()
