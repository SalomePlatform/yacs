import sys
from qt import *

log="""
Ligne 1
Ligne 1
Ligne 1
Ligne 1
Ligne 1
Ligne 1
Ligne 1
Ligne 1
Ligne 1
Ligne 1
Ligne 1
Ligne 1
Ligne 1
"""
class LogView(QDialog):
   def __init__(self,parent = None,name = None,modal = 0,fl = 0):
     QDialog.__init__(self,parent,name,modal,fl)
     layout = QVBoxLayout(self,6,6,"AboutFormLayout")
     self.text=QTextBrowser(self,"log")
     self.text.setFrameShape(QTextEdit.NoFrame)
     self.text.setTextFormat(QTextEdit.PlainText)
     #self.text.setWordWrap(QTextEdit.FixedColumnWidth)
     #self.text.setWrapColumnOrWidth(120)
     self.text.setReadOnly(1)
     #self.text.setText(log)
     layout.addWidget(self.text)
     self.button = QPushButton(self,"pushButton1")
     self.button.setText("&Close")
     layout.addWidget(self.button)
     self.resize(QSize(532,437).expandedTo(self.minimumSizeHint()))
     self.connect(self.button,SIGNAL("clicked()"),self.accept)



if __name__ == "__main__":
  a = QApplication(sys.argv)
  QObject.connect(a,SIGNAL("lastWindowClosed()"),a,SLOT("quit()"))
  w = LogView()
  w.text.setText("un texte extremement loooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooong")
  a.setMainWidget(w)
  w.show()
  a.exec_loop()
