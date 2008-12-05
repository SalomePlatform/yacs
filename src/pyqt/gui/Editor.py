#  Copyright (C) 2006-2008  CEA/DEN, EDF R&D
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

