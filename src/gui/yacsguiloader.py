from salomeloader import *

from YACSGui_Swig import *

import glob
import string

def load(filename):

    SALOMERuntime.RuntimeSALOME_setRuntime()

    loader=SalomeLoader()
    p=loader.load(filename)

    name_list = string.split(filename,"/")
    name = name_list[len(name_list)-1]
    p.setName(name)
    
    gui_swig = YACSGui_Swig()
    gui_swig.displayGraph(p)

    pass
