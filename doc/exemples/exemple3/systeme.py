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

