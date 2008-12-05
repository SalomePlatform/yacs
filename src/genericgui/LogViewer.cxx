//  Copyright (C) 2006-2008  CEA/DEN, EDF R&D
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
#include "LogViewer.hxx"
#include <fstream>
#include <sstream>

using namespace std;

LogViewer::LogViewer(std::string label, QWidget *parent)
{
  setupUi(this);
  _label = label;
  browser->setLineWrapMode(QTextEdit::NoWrap);
  browser->setReadOnly(1);
  connect(pushButton,SIGNAL(clicked()),this, SLOT(close()));
}

LogViewer::~LogViewer()
{
}

void LogViewer::setText(std::string text)
{
  string atext = _label + "\n\n" + text;
  browser->setText(atext.c_str());
}

void LogViewer::readFile(std::string fileName)
{
  std::fstream f(fileName.c_str());
  std::stringstream hfile;
  hfile << f.rdbuf();
  string atext = _label + "\n" + fileName + "\n\n";
  QString qtext = atext.c_str();
  browser->setText(qtext + hfile.str().c_str());
  f.close();
}
