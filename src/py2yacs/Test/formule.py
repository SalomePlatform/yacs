#!/usr/bin/env python3
import yacsdecorator

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
