from SALOME_ComponentPy import *

import FreeFem

class FreeFemComponent(Engines__POA.FreeFemComponent,
                       SALOME_ComponentPy_i):

    def __init__(self, orb, poa, this, containerName,
                 instanceName, interfaceName):
        SALOME_ComponentPy_i.__init__(self, orb, poa, this, containerName,
                                      instanceName, interfaceName, False)
        self.F = FreeFem.FreeFem();

    def Bords(self, b):
        self.F.Bords(b);

    def Flux(self, u1, u2):
        self.F.Flux(u1, u2);

    def Convection(self, cond_init, dt, n):
        self.F.Convection(cond_init, dt, n)

