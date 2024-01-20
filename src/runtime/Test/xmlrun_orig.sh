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
  return args

with open("input",'r') as f:
  data=f.read()
print(data)

class Objref:
  """Wrapper for objrefs """
  def __init__(self,data=None):
    self.data=data
  def __str__(self):
    return self.data or ""

# __cmp__ is not defined in Python 3 : strict ordering
  def __le__(self, other):
    if isinstance(other, Binary):
      other = other.data
    return self.data <= other
  def __lt__(self, other):
    if isinstance(other, Binary):
      other = other.data
    return self.data < other
  def __ge__(self, other):
    if isinstance(other, Binary):
      other = other.data
    return self.data >= other
  def __gt__(self, other):
    if isinstance(other, Binary):
      other = other.data
    return self.data > other
  def __eq__(self, other):
    if isinstance(other, Binary):
      other = other.data
    return self.data == other
  def __ne__(self, other):
    if isinstance(other, Binary):
      other = other.data
    return self.data != other

  def decode(self, data):
    self.data = data

  def encode(self, out):
    out.write("<value><objref>")
    out.write(self.data or "")
    out.write("</objref></value>\n")

xmlrpc.client.WRAPPERS=xmlrpc.client.WRAPPERS+(Objref,)

def end_objref(self,data):
  self.append(Objref(data))
  self._value=0

xmlrpc.client.Unmarshaller.end_objref=end_objref
xmlrpc.client.Unmarshaller.dispatch["objref"]=end_objref

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
with open("output",'w') as f:
  f.write(response)
