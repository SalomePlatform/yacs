#!/usr/bin/env python
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

# Francis KLOSS - CEA/DEN/DANS/DM2S/SFME/LGLS - 2009
# ==================================================

import sys

import PyQt4.Qt

qt = PyQt4.Qt

print "ALARM: popup: started"

# Get parameters
# --------------

if sys.argv[1]=="1":
    c = qt.QColor(0, 255, 0)
else:
    c = qt.QColor(255, 0, 0)

m = sys.argv[2]

# Substitute end of line
# ----------------------

f = m.find("\\n")
while f!=(-1):
    m = m[:f] + "\n" + m[f+2:]
    f = m.find("\\n")

# Create an application
# ---------------------

a = qt.QApplication(sys.argv)

# Create a widget
# ---------------

w = qt.QWidget(None)

l = qt.QVBoxLayout(w)

t = qt.QLabel(m, w)
t.setAutoFillBackground(True)
t.setPalette(qt.QPalette(c))

b = qt.QPushButton("Close", w)
w.connect(b, qt.SIGNAL("clicked()"), a.quit)

l.addWidget(t)
l.addWidget(b)

w.setLayout(l)
w.setWindowTitle("From Yacs Batch")
w.move(600, 400)
w.resize(200, 40)

# Play the interaction
# --------------------

w.show()

a.exec_()

print "ALARM: popup: finished"
