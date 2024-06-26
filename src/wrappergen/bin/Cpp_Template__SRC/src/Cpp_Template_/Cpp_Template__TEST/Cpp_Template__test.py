# Copyright (C) 2006-2024  CEA, EDF
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
print("Test Program of Cpp_Template_ component")

# ...

