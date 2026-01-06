// Copyright (C) 2016-2026  CEA, EDF
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
#include "Py2YacsDialog_raw.hxx"
#include <QApplication>
#include "Proc.hxx"

#include <iostream>
int main(int argc, char *argv[])
{
  QApplication app(argc,argv);
  YACS::ENGINE::RuntimeSALOME::setRuntime();
  Py2YacsDialog_raw mygui;
  if(mygui.exec())
  {
    std::cout << "Accepted" << std::endl;
    YACS::ENGINE::Proc* schema = mygui.getYacsSchema();
    schema->saveSchema("yacs_schema.xml");
    delete schema;
  }
  else
  {
    std::cout << "Not accepted" << std::endl;
  }
  return 0;
}
