import CORBA

orb = CORBA.ORB_init();

f = open('AlgLin.ior');
s = f.read();
f.close();

o = orb.string_to_object(s);

import Distant
o = o._narrow(Distant.AlgLin)


v1 = [ 1, 2, 3 ]
v2 = [ 3, 4, 5 ]

v3 = o.addvec(v1, v2)
print v3

print o.prdscl(v1, v3)
