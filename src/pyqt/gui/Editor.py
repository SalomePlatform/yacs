
from qt import QSizePolicy,QMultiLineEdit
try:
  from qtext import QextScintilla,QextScintillaLexerPython

  class Editor(QextScintilla):
    def __init__(self, parent=None, name=None, flags=0):
      QextScintilla.__init__(self, parent, name, flags)
      self.lexer=QextScintillaLexerPython(self)
      self.setLexer(self.lexer)
      self.lexer.setIndentationWarning(QextScintillaLexerPython.Inconsistent)
      #self.lexer.setAutoIndentStyle(0)
      self.lexer.setAutoIndentStyle(QextScintilla.AiMaintain)
      self.setAutoIndent(1)
      self.setBraceMatching(QextScintilla.SloppyBraceMatch)
      self.setSizePolicy(QSizePolicy(QSizePolicy.Expanding,QSizePolicy.Expanding))
except:
  Editor=QMultiLineEdit

