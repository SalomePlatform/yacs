# -*- coding: utf-8 -*-
# Copyright (C) 2007-2024  CEA, EDF
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
def p1():
  return ["a"], ["b"]

def p2(x):
  return ["a"], ["b"]

def p3(x):
  return 5, 6

def p4(a):
  x= a / 0
  return ["a"], ["b"]

class FunctionProperties:
  def __init__(self, function_name):
    self.name = function_name
    self.inputs=[]
    self.outputs=None
    self.errors=[]
    self.imports=[]
    pass

def p5(f):
  fp = FunctionProperties("boo")
  fp.inputs=["a", 5]
  return [fp], ["a", "b", "c"]

def p6(f):
  fp = FunctionProperties("boo")
  fp.outputs=[7, 5]
  return [fp], ["a", "b", "c"]

def p7(f):
  fp = FunctionProperties("boo")
  fp.errors=[7, 5]
  return [fp], ["a", "b", "c"]

def p8(f):
  fp = FunctionProperties("boo")
  fp.name=[5]
  return [fp], ["a", "b", "c"]

def p9(f):
  fp = FunctionProperties("boo")
  return [fp], "a"

def p10(f):
  fp = FunctionProperties("boo")
  return [fp], ["a", fp, "c"]

