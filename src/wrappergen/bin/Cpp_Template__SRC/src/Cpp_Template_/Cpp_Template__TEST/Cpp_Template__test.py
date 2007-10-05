from os import getenv
if getenv("SALOMEPATH"):
    import salome
    import Cpp_Template__ORB
    my_Cpp_Template_ = salome.lcc.FindOrLoadComponent("FactoryServer", "Cpp_Template_")
    IN_SALOME_GUI = 1
else:
    import Cpp_Template_SWIG
    my_Cpp_Template_=Cpp_Template_SWIG.Cpp_Template_()
pass
#
#
print "Test Program of Cpp_Template_ component"

# ...

