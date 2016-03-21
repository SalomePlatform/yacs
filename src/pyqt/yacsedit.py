# Copyright (C) 2006-2016  CEA/DEN, EDF R&D
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

# -*- coding: iso-8859-1 -*-
#
import sys
import glob
import SALOMERuntime
import pilot
import salomeloader
from gui import Item
from gui import Items
from gui import adapt
from qt import *
from gui.Appli import Appli

SALOMERuntime.RuntimeSALOME_setRuntime()

loader=salomeloader.SalomeLoader()

app = QApplication(sys.argv)
t=Appli()
app.setMainWidget(t)
t.show()
app.exec_loop()

