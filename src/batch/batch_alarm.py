#!/usr/bin/env python

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
