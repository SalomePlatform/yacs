#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# Copyright (C) 2024-2026  CEA, EDF
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
def while_func(context):
    context -= 1
    print("while ", context)
    cond = context > 0
    return cond, context

@yacsdecorator.leaf
def extra_init():
    a = 5
    return a

@yacsdecorator.leaf
def post(x):
    v = x
    return v

@yacsdecorator.leaf
def f1(x):
    y = x + 2
    return y

@yacsdecorator.leaf
def f2(x):
    y = x * 2
    return y

@yacsdecorator.leaf
def g(a, b):
    r = a + b
    print("g(", r, ")")
    cond = r < 100
    return cond, r

@yacsdecorator.block
def while_block(context):
    a = f1(context)
    b = f2(context)
    cond, r = g(a,b)
    return cond, r

@yacsdecorator.block
def main():
    v = extra_init()
    x = yacsdecorator.whileloop(while_func, v)
    post(x)
    yacsdecorator.whileloop(while_func, 3)
    r = yacsdecorator.whileloop(while_block, 1)
    print("r final:", r)

if __name__ == "__main__" :
    main()
