#  Copyright (C) 2006-2010  CEA/DEN, EDF R&D
#
#  This library is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
#  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

from os import system, popen
from string import split

class Systeme:
    
    def __init__(self):
        self.repertoire = '.'

    def cd(self, rep):
        self.repertoire = rep

    def cp(self, nom1, nom2):
        system('cp '
               + self.repertoire + '/' + nom1 + ' ' 
               + self.repertoire + '/' + nom2)

    def touch(self, nom):
        system('touch '
               + self.repertoire + '/' + nom)

    def rm(self, nom):
        system('rm '
               + self.repertoire + '/' + nom)
        
    def dir(self):
        f = popen('ls ' + self.repertoire)
        s = f.read()
        f.close()
        return split(s)

