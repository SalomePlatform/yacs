#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# Copyright (C) 2024  CEA, EDF
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

from salome.yacs import yacsdecorator
import formule

@yacsdecorator.leaf
def f1(x,y):
  r = x+y
  return r

@yacsdecorator.leaf
def f2(a):
  r = a + 2
  return r

@yacsdecorator.leaf
def f3(x, y):
  s = x+y
  p = x*y
  return s,p

@yacsdecorator.leaf
def jdd():
  r = list(range(10))
  return r

@yacsdecorator.foreach
def fr(v):
  a,b = f3(v, 2)
  return a,b

@yacsdecorator.foreach
def fr2(v):
  r = f2(v)
  return r

@yacsdecorator.foreach
def doublefr(v):
  return fr2(v)

@yacsdecorator.leaf
def post(t):
  s = 0
  for e in t:
    s += int( e )
  return s

@yacsdecorator.block
def mainblock():
  return fr(range(10))

@yacsdecorator.block
def maindoublefr():
  vals = [ list(range(x)) for x in range(10)]
  return doublefr(vals)

@yacsdecorator.block
def main():
  vals = jdd()
  result = fr2(vals)
  r1 = post(result)
  x = formule.f1(x=3,y=4)
  a,b = formule.f3(x, 2)
  formule.f2(x)
  r2 = formule.f1(a,b)
  return r1,r2

if __name__ == '__main__':
  v1, v2 = main()
  print("v1:", v1)
  print("v2:", v2)
