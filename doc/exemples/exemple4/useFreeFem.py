from FreeFem import *

C = FreeFem()

C.Flux("y", "-x")
C.Bords( [ Bord("x=cos(2*pi*t)", "y=sin(2*pi*t)",  100)] );
C.Convection('exp(-10*((x-0.3)^2 +(y-0.3)^2))', 0.1, 50)

