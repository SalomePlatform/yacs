# -*- coding: utf-8 -*-
# Copyright (C) 2007-2016  CEA/DEN, EDF R&D
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

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
