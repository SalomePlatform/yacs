import systeme

S = systeme.Systeme()

print "create U1 ..."
S.touch('U1')
print "dir : ", S.dir()

print "copy U1 to U2 ..."
S.cp('U1', 'U2')
print "dir : ", S.dir()

print "delete U1 ..."
S.rm('U1')
print "dir : ", S.dir()

print "delete U2 ..."
S.rm('U2')
print "dir : ", S.dir()
