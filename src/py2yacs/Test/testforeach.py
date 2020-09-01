#!/usr/bin/env python3
import yacsdecorator
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
    s += e
  return s

@yacsdecorator.bloc
def mainbloc():
  return fr(range(10))

@yacsdecorator.bloc
def maindoublefr():
  vals = [ list(range(x)) for x in range(10)]
  return doublefr(vals)

@yacsdecorator.bloc
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
