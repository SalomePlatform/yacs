// Copyright (C) 2006-2016  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#ifndef _FORMHPCONTAINER_HXX_
#define _FORMHPCONTAINER_HXX_

#include "FormContainerBase.hxx"

namespace YACS
{
  namespace ENGINE
  {
    class Container;
  }
}

#ifdef HAS_PYEDITOR
class PyEditor_Editor;
#endif

class QTextEdit;
class QLineEdit;

class FormHPContainer : public FormContainerBase
{
  Q_OBJECT

public:
  FormHPContainer(QWidget *parent = 0);
  virtual ~FormHPContainer();
  void FillPanel(YACS::ENGINE::Container *container);
  QString getTypeStr() const;
  bool onApply();
public slots:
  void onModifySzOfPool(const QString& newSz);
  void initSciptChanged();
public:
  static std::string BuildWithFinalEndLine(const std::string& script);
private:
  QLineEdit *_poolSz;
#ifdef HAS_PYEDITOR
  PyEditor_Editor* _initScript;
#else
  QTextEdit* _initScript;
#endif
  bool _initScriptModified;
  bool _initScriptLoaded;
};

#endif
