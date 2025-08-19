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

@yacsdecorator.block
def b1():
  x = f1(x=3,y=4)
  a,b = f3(x, 2)
  f2(x)
  r = f1(a,b)
  return r

if __name__ == '__main__':
  r = b1()
  print("result:", r)
