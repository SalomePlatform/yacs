//  Copyright (C) 2006-2008  CEA/DEN, EDF R&D, OPEN CASCADE
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
#ifndef YACSGui_Graph_LogViewer
#define YACSGui_Graph_LogViewer

#include <qdialog.h>
#include <qlayout.h>
#include <qtextbrowser.h>
#include <qpushbutton.h>


#include <sstream>

class LogViewer : public QDialog {
public:
    LogViewer(const std::string& txt, QWidget *parent=0, const char *name=0,WFlags f = 0) : QDialog(parent,name,FALSE,f)
      {
        mytext=txt;
        QVBoxLayout* vb = new QVBoxLayout(this,8);
        vb->setAutoAdd(TRUE);
        browser = new QTextBrowser( this );
        browser->setFrameStyle( QFrame::Panel | QFrame::Sunken );
        browser->setTextFormat( QTextEdit::PlainText );
        browser->setWordWrap(QTextEdit::NoWrap);
        browser->setReadOnly(1);
        browser->setText(txt);
        button= new QPushButton("Close",this);
        connect(button,SIGNAL(clicked()),this, SLOT(close()));
        resize(500,500);
      }
    ~LogViewer()
      {
        //std::cerr << "~LogViewer()" << std::endl;
      }
    void readFile(const std::string& filename)
      {
        std::fstream f(filename.c_str());
        std::stringstream hfile;
        hfile << f.rdbuf();
        browser->setText(mytext+"\n\n"+ hfile.str());
        f.close();
      }

private:
    std::string mytext;
    QPushButton* button;
    QTextBrowser* browser;
};

#endif
