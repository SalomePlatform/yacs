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

#ifndef _EDITIONPYFUNC_HXX_
#define _EDITIONPYFUNC_HXX_

#include "EditionScript.hxx"
#include "FormComponent.hxx"
#include <QRadioButton>

namespace YACS
{
  namespace HMI
  {
    class EditionPyFunc: public EditionScript
    {
      Q_OBJECT

    public slots:
      virtual void onApply();
      virtual void onCancel();
      virtual void onFuncNameModified(const QString &text);
      virtual void onTemplate();

    public:
      EditionPyFunc(Subject* subject,
                    QWidget* parent = 0,
                    const char* name = 0);
      virtual ~EditionPyFunc();

    protected:
      SubjectPyFuncNode* _subFuncNode;
      std::string _funcName;
      QLineEdit* _liFuncName;
    };
  }
}
#endif
