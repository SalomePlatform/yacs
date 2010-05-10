import Solveur__POA
import FreeFem

class FreeFem_i(Solveur__POA.FreeFem):

    def __init__(self):
        self.F = FreeFem.FreeFem();

    def Bords(self, b):
        self.F.Bords(b);

    def Flux(self, u1, u2):
        self.F.Flux(u1, u2);

    def Convection(self, cond_init, dt, n):
        self.F.Convection(cond_init, dt, n)

