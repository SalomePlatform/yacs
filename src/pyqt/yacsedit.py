# -*- coding: iso-8859-1 -*-
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

