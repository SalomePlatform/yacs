// Copyright (C) 2016  CEA/DEN, EDF R&D
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
// Author : Anthony Geay (EDF R&D)

#include "YDFXGUIPortsValidator.hxx"

#include <QIntValidator>
#include <QDoubleValidator>

#include "YACSEvalPort.hxx"
#include "YACSEvalSeqAny.hxx"

QValidator *BuildValidatorFromPort(QObject *parent, YACSEvalPort *port)
{
  if(!port)
    return 0;
  std::string tod(port->getTypeOfData());
  if(tod==YACSEvalAnyDouble::TYPE_REPR)
    return new QDoubleValidator(parent);
  if(tod==YACSEvalAnyInt::TYPE_REPR)
    return new QIntValidator(parent);
  return 0;
}

YACSEvalAny *BuildAnyFromString(const QString& text, YACSEvalPort *port)
{
  if(!port)
    return 0;
  std::string tod(port->getTypeOfData());
  if(tod==YACSEvalAnyDouble::TYPE_REPR)
    {
      bool isok;
      double val(text.toDouble(&isok));
      if(!isok)
        return 0;
      return new YACSEvalAnyDouble(val);
    }
  if(tod==YACSEvalAnyInt::TYPE_REPR)
    {
      bool isok;
      int val(text.toInt(&isok));
      if(!isok)
        return 0;
      return new YACSEvalAnyInt(val);
    }
  return 0;
}

QString BuidStringFromAny(YACSEvalAny *val)
{
  if(!val)
    return QString();
  if(val->getTypeOfData()==YACSEvalAnyDouble::TYPE_REPR)
    {
      YACSEvalAnyDouble *valc(static_cast<YACSEvalAnyDouble *>(val));
      return QString::number(valc->toDouble(),'g',16);
    }
  if(val->getTypeOfData()==YACSEvalAnyInt::TYPE_REPR)
    {
      YACSEvalAnyInt *valc(static_cast<YACSEvalAnyInt *>(val));
      return QString::number(valc->toInt());
    }
  return QString();
}

YACSEvalSeqAny *BuildEmptySeqFromPort(YACSEvalPort *port)
{
  std::string tod(port->getTypeOfData());
  if(tod==YACSEvalAnyDouble::TYPE_REPR)
    {
      std::vector<double> v;
      return new YACSEvalSeqAnyDouble(v);
    }
  if(tod==YACSEvalAnyInt::TYPE_REPR)
    {
      std::vector<int> v;
      return new YACSEvalSeqAnyInt(v);
    }
  return 0;
}
