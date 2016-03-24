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

import xmlrpclib,sys

#example
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
  print "args=",args
  if not args:
    return None
  elif len(args) == 1:
    return args[0]
  else:
    return args

f=open("input")
data=f.read()
f.close()
print data

class Objref:
  """Wrapper for objrefs """
  def __init__(self,data=None):
    self.data=data
  def __str__(self):
    return self.data or ""
  def __cmp__(self, other):
    if isinstance(other, Binary):
      other = other.data
    return cmp(self.data, other)

  def decode(self, data):
    self.data = data

  def encode(self, out):
    out.write("<value><objref>")
    out.write(self.data or "")
    out.write("</objref></value>\n")

xmlrpclib.WRAPPERS=xmlrpclib.WRAPPERS+(Objref,)

def end_objref(self,data):
  self.append(Objref(data))
  self._value=0

xmlrpclib.Unmarshaller.end_objref=end_objref
xmlrpclib.Unmarshaller.dispatch["objref"]=end_objref

params, method = xmlrpclib.loads(data)

try:
   call=eval(method)
   response=call(params)
   response = (response,)
except:
   # report exception back to server
   response = xmlrpclib.dumps( xmlrpclib.Fault(1, "%s:%s" % sys.exc_info()[:2]))
else:
   response = xmlrpclib.dumps( response, methodresponse=1)

print response
f=open("output",'w')
f.write(response)
f.close()
