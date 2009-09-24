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
#include "FormContainer.hxx"
#include "FormComponent.hxx"
#include "QtGuiContext.hxx"
#include "Container.hxx"

#include <cassert>
#include <cstdlib>
#include <climits>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

#include <QList>
#include <sstream>

using namespace std;
using namespace YACS;
using namespace YACS::HMI;
using namespace YACS::ENGINE;

bool FormContainer::_checked = false;

FormContainer::FormContainer(QWidget *parent)
{
  setupUi(this);
  _advanced = false;
  _properties.clear();

  gridLayout->removeWidget(cb_host);
  delete cb_host;
  cb_host = new ComboBox(gb_basic);
  gridLayout->addWidget(cb_host, 1, 1, 1, 1);

  QIcon icon;
  icon.addFile("icons:icon_down.png");
  icon.addFile("icons:icon_up.png",
                QSize(), QIcon::Normal, QIcon::On);
  tb_container->setIcon(icon);
  on_tb_container_toggled(FormContainer::_checked);
  on_ch_advance_stateChanged(0);

  sb_mem->setMaximum(INT_MAX);
  sb_cpu->setMaximum(INT_MAX);
  sb_nbNodes->setMaximum(INT_MAX);
  sb_procNode->setMaximum(INT_MAX);

  FillPanel(0); // --- set widgets before signal connexion to avoid false modif detection

  connect(le_name, SIGNAL(textChanged(const QString&)),
          this, SLOT(onModifyName(const QString&)));

  connect(cb_host, SIGNAL(activated(const QString&)),
          this, SLOT(onModifyHost(const QString&)));

  connect(cb_policy, SIGNAL(activated(const QString&)),
          this, SLOT(onModifyPolicy(const QString&)));

  connect(cb_type, SIGNAL(activated(const QString&)),
          this, SLOT(onModifyType(const QString&)));

  connect(le_workdir, SIGNAL(textChanged(const QString&)),
          this, SLOT(onModifyWorkDir(const QString&)));

  connect(le_contname, SIGNAL(textChanged(const QString&)),
          this, SLOT(onModifyContName(const QString&)));

  connect(le_os, SIGNAL(textChanged(const QString&)),
          this, SLOT(onModifyOS(const QString&)));

  connect(le_parallel, SIGNAL(textChanged(const QString&)),
          this, SLOT(onModifyParLib(const QString&)));

  connect(ch_mpi, SIGNAL(clicked(bool)),
          this, SLOT(onModifyIsMPI(bool)));

  connect(sb_mem, SIGNAL(valueChanged(const QString&)),
          this, SLOT(onModifyMem(const QString&)));

  connect(sb_cpu, SIGNAL(valueChanged(const QString&)),
          this, SLOT(onModifyClock(const QString&)));

  connect(sb_nbNodes, SIGNAL(valueChanged(const QString&)),
          this, SLOT(onModifyNodes(const QString&)));

  connect(sb_procNode, SIGNAL(valueChanged(const QString&)),
          this, SLOT(onModifyProcs(const QString&)));

  connect(sb_nbCompoNodes, SIGNAL(valueChanged(const QString&)),
          this, SLOT(onModifyCompos(const QString&)));
}

FormContainer::~FormContainer()
{
}

void FormContainer::FillPanel(YACS::ENGINE::Container *container)
{
  _container = container;
  if (_container)
    {
      _properties = _container->getProperties();
      le_name->setText(_container->getName().c_str());
    }
  else
    {
      _properties.clear();
      le_name->setText("not defined");
    }

  cb_type->clear();
  cb_type->addItem("mono");
  cb_type->addItem("multi");
  int i=0;
  if(_properties.count("type") && _properties["type"]=="multi")i=1;
  cb_type->setCurrentIndex(i);

  vector<string> policies;
  policies.push_back("cycl");
  policies.push_back("altcycl");
  policies.push_back("best");
  policies.push_back("first");
  cb_policy->clear();
  for(int i=0; i< policies.size(); i++)
    cb_policy->addItem(policies[i].c_str());
  if(_properties.count("policy"))
    {
      int i=0;
      for(i=0; i< policies.size(); i++)
        if(policies[i] == _properties["policy"])
          {
            cb_policy->setCurrentIndex(i);
            break;
          }
    }
  else
    cb_policy->setCurrentIndex(0);
  
  cb_host->clear();
  cb_host->addItem("automatic"); // --- when no host selected

  list<string> machines = QtGuiContext::getQtCurrent()->getGMain()->getMachineList();
  list<string>::iterator itm = machines.begin();
  for( ; itm != machines.end(); ++itm)
    {
      cb_host->addItem(QString((*itm).c_str()));
    }
  if(_properties.count("hostname") && _properties["hostname"] != "")
    {
      int index = cb_host->findText(_properties["hostname"].c_str());
      if (index >= 0)
        cb_host->setCurrentIndex(index);
      else
        {
          cb_host->addItem(_properties["hostname"].c_str());
          cb_host->setCurrentIndex(cb_host->count()-1);
        }
    }
  else
    cb_host->setCurrentIndex(0);


  if(_properties.count("workingdir"))
    le_workdir->setText(_properties["workingdir"].c_str());
  else
    le_workdir->setText("");

  if(_properties.count("container_name"))
    le_contname->setText(_properties["container_name"].c_str());
  else
    le_contname->setText("");
                     
  if(_properties.count("OS"))
    le_os->setText(_properties["OS"].c_str());
  else
    le_os->setText("");

  if(_properties.count("parallelLib"))
    le_parallel->setText(_properties["parallelLib"].c_str());
  else
    le_parallel->setText("");

  if(_properties.count("isMPI"))
    {
      DEBTRACE("_properties[isMPI]=" << _properties["isMPI"]);
      if ((_properties["isMPI"] == "0") || (_properties["isMPI"] == "false"))
        ch_mpi->setCheckState(Qt::Unchecked);
      else
        ch_mpi->setCheckState(Qt::Checked);
    }
  else
    ch_mpi->setCheckState(Qt::Unchecked);

  if(_properties.count("mem_mb"))
    sb_mem->setValue(atoi(_properties["mem_mb"].c_str()));
  else
    sb_mem->setValue(0);

  if(_properties.count("cpu_clock"))
    sb_cpu->setValue(atoi(_properties["cpu_clock"].c_str()));
  else
    sb_cpu->setValue(0);


  if(_properties.count("nb_node"))
    sb_nbNodes->setValue(atoi(_properties["nb_node"].c_str()));
  else
    sb_nbNodes->setValue(0);

  if(_properties.count("nb_proc_per_node"))
    sb_procNode->setValue(atoi(_properties["nb_proc_per_node"].c_str()));
  else
    sb_procNode->setValue(0);

  if(_properties.count("nb_component_nodes"))
    sb_nbCompoNodes->setValue(atoi(_properties["nb_component_nodes"].c_str()));
  else
    sb_nbCompoNodes->setValue(0);
}

void FormContainer::onModified()
{
  DEBTRACE("FormContainer::onModified");
  Subject *sub = QtGuiContext::getQtCurrent()->getSelectedSubject();
  if (!sub) return;
  int idEd = QtGuiContext::getQtCurrent()->_mapOfEditionItem[sub];
  QStackedWidget *widStack = QtGuiContext::getQtCurrent()->getStackedWidget();
  QWidget *widget = widStack->widget(idEd);
  ItemEdition *item = dynamic_cast<ItemEdition*>(widget);
  YASSERT(item);
  item->setEdited(true);
}

void FormContainer::on_tb_container_toggled(bool checked)
{
  DEBTRACE("FormContainer::on_tb_container_toggled " << checked);
  _checked = checked;
  if (_checked) fr_container->show();
  else fr_container->hide();
}

void FormContainer::on_ch_advance_stateChanged(int state)
{
  //DEBTRACE("FormContainer::on_ch_advance_stateChanged " << state);
  if (state) gb_advance->show();
  else gb_advance->hide();
}

void FormContainer::onModifyName(const QString &text)
{
  DEBTRACE("onModifyName " << text.toStdString());
  SubjectContainer *scont =
    QtGuiContext::getQtCurrent()->_mapOfSubjectContainer[_container];
  YASSERT(scont);
  string name = scont->getName();
  if (name != text.toStdString())
    onModified();
}

void FormContainer::onModifyHost(const QString &text)
{
  DEBTRACE("onModifyHost " << text.toStdString());
  if (!_container) return;
  std::string host=text.toStdString();
  if(host=="automatic")host="";
  map<string,string> properties = _container->getProperties();
  _properties["hostname"] = host;
  if (properties["hostname"] != host)
    onModified();
}

void FormContainer::onModifyType(const QString &text)
{
  DEBTRACE("onModifyType " << text.toStdString());
  if (!_container) return;
  std::string prop=_container->getProperty("type");
  _properties["type"] = text.toStdString();
  if (prop != text.toStdString())
    onModified();
}

void FormContainer::onModifyPolicy(const QString &text)
{
  DEBTRACE("onModifyPolicy " << text.toStdString());
  if (!_container) return;
  map<string,string> properties = _container->getProperties();
  _properties["policy"] = text.toStdString();
  if (properties["policy"] != text.toStdString())
    {
      onModified();
    }
}

void FormContainer::onModifyWorkDir(const QString &text)
{
  DEBTRACE("onModifyWorkDir " << text.toStdString());
  if (!_container) return;
  map<string,string> properties = _container->getProperties();
  _properties["workingdir"] = text.toStdString();
  if (properties["workingdir"] != text.toStdString())
    {
      onModified();
    }
}

void FormContainer::onModifyContName(const QString &text)
{
  DEBTRACE("onModifyContName " << text.toStdString());
  if (!_container) return;
  map<string,string> properties = _container->getProperties();
  _properties["container_name"] = text.toStdString();
  if (properties["container_name"] != text.toStdString())
    {
      onModified();
    }
}

void FormContainer::onModifyOS(const QString &text)
{
  DEBTRACE("onModifyOS " << text.toStdString());
  if (!_container) return;
  map<string,string> properties = _container->getProperties();
  _properties["OS"] = text.toStdString();
  if (properties["OS"] != text.toStdString())
    {
      onModified();
    }
}

void FormContainer::onModifyParLib(const QString &text)
{
  DEBTRACE("onModifyParLib " << text.toStdString());
  if (!_container) return;
  map<string,string> properties = _container->getProperties();
  _properties["parallelLib"] = text.toStdString();
  if (properties["parallelLib"] != text.toStdString())
    {
      onModified();
    }
}

void FormContainer::onModifyIsMPI(bool isMpi)
{
  DEBTRACE("onModifyIsMPI " << isMpi);
  if (!_container) return;
  string text = "false";
  if (isMpi) text = "true";
  DEBTRACE(text);
  map<string,string> properties = _container->getProperties();
  _properties["isMPI"] = text;
  if (properties["isMPI"] != text)
    {
      onModified();
    }
}

void FormContainer::onModifyMem(const QString &text)
{
  DEBTRACE("onModifyMem " << text.toStdString());
  if (!_container) return;
  map<string,string> properties = _container->getProperties();
  _properties["mem_mb"] = text.toStdString();
  if (properties["mem_mb"] != text.toStdString())
    {
      onModified();
    }
}

void FormContainer::onModifyClock(const QString &text)
{
  DEBTRACE("onModifyClock " << text.toStdString());
  if (!_container) return;
  map<string,string> properties = _container->getProperties();
  _properties["cpu_clock"] = text.toStdString();
  if (properties["cpu_clock"] != text.toStdString())
    {
      onModified();
    }
}

void FormContainer::onModifyNodes(const QString &text)
{
  DEBTRACE("onModifyNodes " << text.toStdString());
  if (!_container) return;
  map<string,string> properties = _container->getProperties();
  _properties["nb_node"] = text.toStdString();
  if (properties["nb_node"] != text.toStdString())
    {
      onModified();
    }
}

void FormContainer::onModifyProcs(const QString &text)
{
  DEBTRACE("onModifyProcs " << text.toStdString());
  if (!_container) return;
  map<string,string> properties = _container->getProperties();
  _properties["nb_proc_per_node"] = text.toStdString();
  if (properties["nb_proc_per_node"] != text.toStdString())
    {
      onModified();
    }
}

void FormContainer::onModifyCompos(const QString &text)
{
  DEBTRACE("onModifyCompo " << text.toStdString());
  if (!_container) return;
  map<string,string> properties = _container->getProperties();
  _properties["nb_component_nodes"] = text.toStdString();
  if (properties["nb_component_nodes"] != text.toStdString())
    {
      onModified();
    }
}

bool FormContainer::onApply()
{
  SubjectContainer *scont =
    QtGuiContext::getQtCurrent()->_mapOfSubjectContainer[_container];
  YASSERT(scont);
  bool ret = scont->setProperties(_properties);
  DEBTRACE(ret);
  if (ret) ret = scont->setName(le_name->text().toStdString());
  return ret;
}
