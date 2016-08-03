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

#include "YDFXGUIMain.hxx"
#include "YDFXGUIPortsSelector.hxx"
#include "YDFXGUIHostParametrizer.hxx"
#include "YDFXGUISeqInit.hxx"
#include "YDFXGUIPushButtons.hxx"

#include "YACSEvalYFX.hxx"

#include <QFileDialog>

/////////////

YDFXGUIMainEFXWidget::YDFXGUIMainEFXWidget(YACSEvalSession *session, YACSEvalYFXWrap *efx, QWidget *parent):QWidget(parent),_efx(efx),_ports(new YDFXGUIAllPorts(efx,this)),_run(0)
{
  QVBoxLayout *mainLayout(new QVBoxLayout(this));
  mainLayout->addWidget(_ports,1);
  QHBoxLayout *buttonLayout(new QHBoxLayout);
  QSpacerItem *si(new QSpacerItem(40,20,QSizePolicy::Expanding,QSizePolicy::Minimum));
  buttonLayout->addItem(si);
  _seqInit=new YDFXGUISeqInitButton(this,_efx,_ports);
  buttonLayout->addWidget(_seqInit);
  _run=new YDFXGUIRunButton(this,session,_efx);
  _run->setEnabled(false);
  connect(_seqInit,SIGNAL(sequenceWellDefined(bool)),_ports,SLOT(somethingChangedInPorts(bool)));
  connect(_ports,SIGNAL(sequencesCanBeDefinedSignal(bool)),_seqInit,SLOT(setEnabled(bool)));
  connect(_ports,SIGNAL(canBeExecutedSignal(bool)),_run,SLOT(setEnabled(bool)));
  //
  buttonLayout->addWidget(_run);
  mainLayout->addLayout(buttonLayout);
}

YDFXGUIMainEFXWidget::~YDFXGUIMainEFXWidget()
{
  delete _efx;
}

AddTabWidget::AddTabWidget(QWidget *parent):QWidget(parent)
{
  QVBoxLayout *mainLayout(new QVBoxLayout);
  QPushButton *pb(new QPushButton("Add from XML file",this));
  mainLayout->addWidget(pb);
  connect(pb,SIGNAL(clicked(bool)),this,SIGNAL(addNewTab()));
  this->setLayout(mainLayout);
}

/////////////

TabEFXViews::TabEFXViews(QWidget *parent, YACSEvalSession *session):QTabWidget(parent),_addWidget(new AddTabWidget(this)),_session(session)
{
  this->addTab(_addWidget,"+");
  connect(_addWidget,SIGNAL(addNewTab()),this,SLOT(newTabFromXMLRequested()));
  this->setTabsClosable(true);
  connect(this,SIGNAL(tabCloseRequested(int)),this,SLOT(closeTabPlease(int)));
}

void TabEFXViews::newTabFromXMLRequested()
{
  QFileDialog fd(this);
  fd.setNameFilter("YACS XML files (*.xml)");
  if(fd.exec())
    {
      QStringList fileNames(fd.selectedFiles());
      if(fileNames.size()!=1)
        return ;
      QString fileName(fileNames[0]);
      QFileInfo fn(fileName);
      YACSEvalYFXWrap *efx(new YACSEvalYFXWrap(YACSEvalYFX::BuildFromFile(fileName.toStdString())));
      YDFXGUIMainEFXWidget *newTab(new YDFXGUIMainEFXWidget(_session,efx,this));
      int index(this->insertTab(count()-1,newTab,fn.baseName()));
      this->setCurrentIndex(index);
    }
}

void TabEFXViews::closeTabPlease(int tabId)
{
  if(tabId==count()-1)
    return ;
  QWidget *tab(this->widget(tabId));
  delete tab;
}

/////////////

YDFXGUI::YDFXGUI(YACSEvalSession *session):_tabWidget(new TabEFXViews(this,session))
{
  QVBoxLayout *mainLayout(new QVBoxLayout);
  mainLayout->addWidget(_tabWidget);
  this->setLayout(mainLayout);
}
