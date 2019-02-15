def addvec(x, y):    
    n = len(x)
    z = []
    for i in range(n):
         z.append(x[i] + y[i])
    return z

def prdscl(x, y):
    n = len(x)
    S = 0
    for i in range(n):
         S = S + x[i] * y[i]
    return S
