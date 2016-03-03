# -*- coding: utf-8 -*-

import os,sys,subprocess,shutil

# Here the most beautiful part of test :)
dirALaCon0="Test"
dirAlaCon1="samples"
if not os.path.exists(dirALaCon0):
    os.mkdir(dirALaCon0)
if not os.path.exists(os.path.join(dirALaCon0,dirAlaCon1)):
    os.chdir(dirALaCon0)
    os.symlink(os.path.join("..",dirAlaCon1),dirAlaCon1)
    os.chdir("..")
# GO !
dn=os.path.dirname(__file__)
p=subprocess.Popen(["python","PMMLBasicsTest.py"],cwd=dn,stdout=subprocess.PIPE,stderr=subprocess.PIPE)
a,b=p.communicate()
ret=p.returncode
# Clean up the wonderful first part stuf
shutil.rmtree(dirALaCon0)
sys.exit(ret)
