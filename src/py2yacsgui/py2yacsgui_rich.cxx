// Copyright (C) 2016-2017  CEA/DEN, EDF R&D
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
#include "RuntimeSALOME.hxx"
#include "Py2YacsDialog.hxx"
#include <QApplication>
#include "Proc.hxx"
#include <PyEditor_StdSettings.h>
#include <iostream>

int main(int argc, char *argv[])
{
  QApplication app(argc,argv);
  app.setOrganizationName( "salome" );
  app.setOrganizationDomain( "www.salome-platform.org" );
  app.setApplicationName( "py2yacs" );

  YACS::ENGINE::RuntimeSALOME::setRuntime();
  Py2YacsDialog mygui;
  if(mygui.exec())
  {
    std::cout << "Accepted:" << mygui.getYacsFile().toStdString() << std::endl;
  }
  else
  {
    std::cout << "Not accepted" << std::endl;
  }
  return 0;
}