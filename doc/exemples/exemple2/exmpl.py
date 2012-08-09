import compo

x = []
y = []
for i in range(5):
    ii = i+1
    x.append(2*ii)
    y.append(ii*ii)

C = compo.compo()
    
z = C.addvec(x, y)
print 'x = ', x
print 'y = ', y
print 'z = ', z
print C.prdscl(x,y)

