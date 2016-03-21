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
  std::ifstream f(fileName.c_str());
  std::stringstream hfile;
  hfile << f.rdbuf();
  string atext = _label + "\n" + fileName + "\n\n";
  QString qtext = atext.c_str();
  browser->setText(qtext + hfile.str().c_str());
  f.close();
}

ContainerLogViewer::ContainerLogViewer(std::string label, QWidget *parent):LogViewer(label,parent)
{
  gridLayout->removeWidget(browser);
  gridLayout->addWidget(browser, 0, 0, 1, 3);

  QPushButton* next = new QPushButton("next execution",this);
  gridLayout->addWidget(next, 1, 1, 1, 1);
  connect(next,SIGNAL(clicked()),this, SLOT(onNext()));
  QPushButton* previous = new QPushButton("previous execution",this);
  gridLayout->addWidget(previous, 1, 2, 1, 1);
  connect(previous,SIGNAL(clicked()),this, SLOT(onPrevious()));
}

void ContainerLogViewer::readFile(std::string fileName)
{
  LogViewer::readFile(fileName);
  //move cursor to the end
  QTextCursor cursor=browser->textCursor();
  cursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
  browser->setTextCursor(cursor);
}

//go to next begin execution schema
void ContainerLogViewer::onNext()
{
  QTextCursor orig = browser->textCursor();
  QTextCursor find_result = browser->document()->find("**************************Begin schema execution", orig);

  if(find_result.isNull())
    {
      //Try to find from the beginning
      find_result = browser->document()->find("**************************Begin schema execution",0);
    }

  if(!find_result.isNull())
    browser->setTextCursor(find_result);
}

//go to previous begin execution schema
void ContainerLogViewer::onPrevious()
{
  QTextCursor orig = browser->textCursor();
  QTextCursor find_result = browser->document()->find("**************************Begin schema execution",
                                                      orig, QTextDocument::FindBackward);
  if(find_result.isNull())
    {
      //Try to find from the end
      find_result = browser->document()->find("**************************Begin schema execution",
                                              browser->document()->characterCount()-1, QTextDocument::FindBackward);
    }

  if(!find_result.isNull())
    browser->setTextCursor(find_result);
}
