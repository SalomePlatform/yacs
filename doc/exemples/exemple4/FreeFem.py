import os

class Bord:
    def __init__(self, X, Y, n):
        self.X = X
        self.Y = Y
        self.n = n
    
class FreeFem:
    def __init__(self):
        self.u1 = "0"
        self.u2 = "0"
        self.bords = [ Bord("x = cos(2*pi*t)", "y = sin(2*pi*t)", 100) ]
        
    def Bords(self, b):
        self.bords = b

    def Flux(self, u1, u2):
        self.u1 = u1
        self.u2 = u2
        
    def Convection(self, cond_init, dt, n):

        f = open("/tmp/example.edp", "w")
        s = ""
        ib = 1
        nb = len(self.bords)
        for b in self.bords:
            f.write("border b" + str(ib) + "(t=0,1){" + \
                    b.X + "; " + b.Y + "; };\n");
            s = s + "b" + str(ib) + "(" + str(b.n)+ ")"
            if (ib < nb):
                s = s + "+ "
            else:
                s = s + ");"
            ib = ib+1
            
        f.write("mesh th = buildmesh(" + s + "\n");
        f.write("fespace Vh(th,P1);\n");
        f.write("Vh v = " + cond_init + ";\nplot(v);\n")
        f.write("real dt = " + str(dt) + ", t=0;\n");
        
        f.write("Vh u1 = " + str(self.u1) + \
                ", u2 = " + str(self.u2) + ";\n");
        
        f.write("int i;\nVh vv,vo;\n"
                "for ( i=0; i< " + str(n) + " ; i++) {\n"
                "t += dt;\nvo=v;\nv=convect([u1,u2],-dt,vo);\n"
                "plot(v,wait=0);\n};\n");
        f.close()
        os.system('FreeFem++ /tmp/example.edp');

