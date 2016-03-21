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

#include "FormAdvParamContainer.hxx"
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

FormAdvParamContainer::FormAdvParamContainer(std::map<std::string, std::string>& prop, QWidget *parent):QWidget(parent),_properties(prop)
{
  setupUi(this);
  sb_mem->setMaximum(INT_MAX);
  sb_cpu->setMaximum(INT_MAX);
  sb_nbNodes->setMaximum(INT_MAX);
  sb_procNode->setMaximum(INT_MAX);
  sb_nbprocpar->setMaximum(INT_MAX);
  sb_nbproc->setMaximum(INT_MAX);
  
  FillPanel("",0); // --- set widgets before signal connexion to avoid false modif detection

  connect(cb_policy, SIGNAL(activated(const QString&)), this, SLOT(onModifyPolicy(const QString&)));
  connect(cb_parallel, SIGNAL(activated(const QString&)), this, SLOT(onModifyParLib(const QString&)));
  connect(le_workdir, SIGNAL(textChanged(const QString&)), this, SLOT(onModifyWorkDir(const QString&)));
  connect(le_contname, SIGNAL(textChanged(const QString&)), this, SLOT(onModifyContName(const QString&)));
  connect(le_os, SIGNAL(textChanged(const QString&)), this, SLOT(onModifyOS(const QString&)));
  connect(le_hostname, SIGNAL(textChanged(const QString&)), this, SLOT(onModifyHostName(const QString&)));
  connect(le_compolist, SIGNAL(textChanged(const QString&)), this, SLOT(onModifyCompoList(const QString&)));
  connect(le_resourceList, SIGNAL(textChanged(const QString&)), this, SLOT(onModifyResourceList(const QString&)));
  connect(ch_mpi, SIGNAL(clicked(bool)), this, SLOT(onModifyIsMPI(bool)));
  connect(sb_mem, SIGNAL(valueChanged(const QString&)), this, SLOT(onModifyMem(const QString&)));
  connect(sb_cpu, SIGNAL(valueChanged(const QString&)), this, SLOT(onModifyClock(const QString&)));
  connect(sb_nbNodes, SIGNAL(valueChanged(const QString&)), this, SLOT(onModifyNodes(const QString&)));
  connect(sb_procNode, SIGNAL(valueChanged(const QString&)), this, SLOT(onModifyProcs(const QString&)));
  connect(sb_nbprocpar, SIGNAL(valueChanged(const QString&)), this, SLOT(onModifyProcPar(const QString&)));
  connect(sb_nbproc, SIGNAL(valueChanged(const QString&)), this, SLOT(onModifyProcRes(const QString&)));
}

FormAdvParamContainer::~FormAdvParamContainer()
{
}

void FormAdvParamContainer::FillPanel(const std::string& resource, YACS::ENGINE::Container *container)
{
  DEBTRACE("FormContainer::FillPanel");
  _container = container;
  if (_container)
    {
      _properties = _container->getProperties();
    }
  else
    {
      _properties.clear();
    }

  if(_properties.count("type") && _properties["type"]=="multi")
    cb_mode->setText("multi");
  else
    cb_mode->setText("mono");

  vector<string> parlibs;
  parlibs.push_back("");
  parlibs.push_back("Mpi");
  parlibs.push_back("Dummy");
  cb_parallel->clear();
  for(int i=0; i< parlibs.size(); i++)
    cb_parallel->addItem(parlibs[i].c_str());
  if(_properties.count("parallelLib"))
    {
      int i=0;
      for(i=0; i< parlibs.size(); i++)
        if(parlibs[i] == _properties["parallelLib"])
          {
            cb_parallel->setCurrentIndex(i);
            break;
          }
    }
  else
    cb_parallel->setCurrentIndex(0);
  
  if(_properties.count("workingdir"))
    le_workdir->setText(_properties["workingdir"].c_str());
  else
    le_workdir->setText("");

  if(_properties.count("container_name"))
    le_contname->setText(_properties["container_name"].c_str());
  else
    le_contname->setText("");

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

  if(_properties.count("nb_parallel_procs"))
    sb_nbprocpar->setValue(atoi(_properties["nb_parallel_procs"].c_str()));
  else
    sb_nbprocpar->setValue(0);
  
  updateResource(resource);

  if (!QtGuiContext::getQtCurrent()->isEdition())
    {
      //if the schema is in execution do not allow editing
      cb_parallel->setEnabled(false);
      le_workdir->setReadOnly(true);
      le_contname->setReadOnly(true);
      ch_mpi->setEnabled(false);
      sb_nbprocpar->setReadOnly(true);
      le_hostname->setEnabled(false);
      le_os->setEnabled(false);
      sb_nbproc->setEnabled(false);
      sb_mem->setEnabled(false);
      sb_cpu->setEnabled(false);
      sb_nbNodes->setEnabled(false);
      sb_procNode->setEnabled(false);
      cb_policy->setEnabled(false);
      le_compolist->setEnabled(false);
      le_resourceList->setEnabled(false);
    }
}

void FormAdvParamContainer::onModified()
{
  DEBTRACE("FormContainer::onModified");
  Subject *sub = QtGuiContext::getQtCurrent()->getSelectedSubject();
  if (!sub) return;
  YASSERT(QtGuiContext::getQtCurrent()->_mapOfEditionItem.count(sub));
  QWidget *widget = QtGuiContext::getQtCurrent()->_mapOfEditionItem[sub];
  ItemEdition *item = dynamic_cast<ItemEdition*>(widget);
  YASSERT(item);
  item->setEdited(true);
}

void FormAdvParamContainer::updateResource(const std::string &resource)
{
  DEBTRACE("FormContainer::updateResource " << resource);
  if(resource.empty())
  {
    //the resource is not specified: use automatic and allow editing
    if(_properties.count("hostname"))
      le_hostname->setText(_properties["hostname"].c_str());
    else
      le_hostname->setText("");
    le_hostname->setEnabled(true);

    if(_properties.count("OS"))
      le_os->setText(_properties["OS"].c_str());
    else
      le_os->setText("");
    le_os->setEnabled(true);

    if(_properties.count("nb_resource_procs"))
      sb_nbproc->setValue(atoi(_properties["nb_resource_procs"].c_str()));
    else
      sb_nbproc->setValue(0);
    sb_nbproc->setEnabled(true);

    if(_properties.count("mem_mb"))
      sb_mem->setValue(atoi(_properties["mem_mb"].c_str()));
    else
      sb_mem->setValue(0);
    sb_mem->setEnabled(true);

    if(_properties.count("cpu_clock"))
      sb_cpu->setValue(atoi(_properties["cpu_clock"].c_str()));
    else
      sb_cpu->setValue(0);
    sb_cpu->setEnabled(true);

    if(_properties.count("nb_node"))
      sb_nbNodes->setValue(atoi(_properties["nb_node"].c_str()));
    else
      sb_nbNodes->setValue(0);
    sb_nbNodes->setEnabled(true);

    if(_properties.count("nb_proc_per_node"))
      sb_procNode->setValue(atoi(_properties["nb_proc_per_node"].c_str()));
    else
      sb_procNode->setValue(0);
    sb_procNode->setEnabled(true);

    std::vector<std::string> policies;
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
      cb_policy->setCurrentIndex(1);
    cb_policy->setEnabled(true);

    if(_properties.count("component_list"))
      le_compolist->setText(_properties["component_list"].c_str());
    else
      le_compolist->setText("");
    le_compolist->setEnabled(true);

    if(_properties.count("resource_list"))
      le_resourceList->setText(_properties["resource_list"].c_str());
    else
      le_resourceList->setText("");
    le_resourceList->setEnabled(true);
  }
  else
  {
    //a specific resource has been chosen: properties are those declared in the resources manager
    //properties can not be edited
    std::map<std::string,std::string> properties(_container->getResourceProperties(resource));
    if(properties.count("hostname"))
      le_hostname->setText(properties["hostname"].c_str());
    else
      le_hostname->setText("");
    le_hostname->setEnabled(false);

    if(properties.count("OS"))
      le_os->setText(properties["OS"].c_str());
    else
      le_os->setText("");
    le_os->setEnabled(false);

    if(properties.count("nb_resource_procs"))
      sb_nbproc->setValue(atoi(properties["nb_resource_procs"].c_str()));
    else
      sb_nbproc->setValue(0);
    sb_nbproc->setEnabled(false);

    if(properties.count("mem_mb"))
      sb_mem->setValue(atoi(properties["mem_mb"].c_str()));
    else
      sb_mem->setValue(0);
    sb_mem->setEnabled(false);

    if(properties.count("cpu_clock"))
      sb_cpu->setValue(atoi(properties["cpu_clock"].c_str()));
    else
      sb_cpu->setValue(0);
    sb_cpu->setEnabled(false);

    if(properties.count("nb_node"))
      sb_nbNodes->setValue(atoi(properties["nb_node"].c_str()));
    else
      sb_nbNodes->setValue(0);
    sb_nbNodes->setEnabled(false);

    if(properties.count("nb_proc_per_node"))
      sb_procNode->setValue(atoi(properties["nb_proc_per_node"].c_str()));
    else
      sb_procNode->setValue(0);
    sb_procNode->setEnabled(false);

    cb_policy->clear();
    cb_policy->setEnabled(false);

    if(properties.count("component_list"))
      le_compolist->setText(properties["component_list"].c_str());
    else
      le_compolist->setText("");
    le_compolist->setEnabled(false);

    if(properties.count("resource_list"))
      le_resourceList->setText(properties["resource_list"].c_str());
    else
      le_resourceList->setText("");
    le_resourceList->setEnabled(false);
  }
}

void FormAdvParamContainer::setModeText(const std::string& mode)
{
  cb_mode->setText(mode.c_str());
}

void FormAdvParamContainer::onModifyResource(const QString &text)
{
  DEBTRACE("onModifyResource " << text.toStdString());
  if (!_container) return;
  std::string resource=text.toStdString();
  if(resource=="automatic")resource="";
  map<string,string> properties = _container->getProperties();
  _properties["name"] = resource;
  if (properties["name"] != resource)
    {
      // reset resource properties
      _properties.erase("hostname");
      _properties.erase("OS");
      _properties.erase("nb_resource_procs");
      _properties.erase("mem_mb");
      _properties.erase("cpu_clock");
      _properties.erase("nb_node");
      _properties.erase("nb_proc_per_node");
      _properties.erase("policy");
      onModified();
      updateResource(resource);
    }
}

void FormAdvParamContainer::onModifyPolicy(const QString &text)
{
  DEBTRACE("onModifyPolicy " << text.toStdString());
  if (!_container) return;
  if(_properties.count("name") && _properties["name"] != "")return; //do not modify resource parameter when specific resource is set
  map<string,string> properties = _container->getProperties();
  _properties["policy"] = text.toStdString();
  if (properties["policy"] != text.toStdString())
    onModified();
}

void FormAdvParamContainer::onModifyWorkDir(const QString &text)
{
  DEBTRACE("onModifyWorkDir " << text.toStdString());
  if (!_container) return;
  map<string,string> properties = _container->getProperties();
  _properties["workingdir"] = text.toStdString();
  if (properties["workingdir"] != text.toStdString())
    onModified();
}

void FormAdvParamContainer::onModifyContName(const QString &text)
{
  DEBTRACE("onModifyContName " << text.toStdString());
  if (!_container) return;
  map<string,string> properties = _container->getProperties();
  _properties["container_name"] = text.toStdString();
  if (properties["container_name"] != text.toStdString())
    onModified();
}

void FormAdvParamContainer::onModifyOS(const QString &text)
{
  DEBTRACE("onModifyOS " << text.toStdString());
  if (!_container) return;
  if(_properties.count("name") && _properties["name"] != "")return; //do not modify resource parameter when specific resource is set
  map<string,string> properties = _container->getProperties();
  _properties["OS"] = text.toStdString();
  if (properties["OS"] != text.toStdString())
    onModified();
}

void FormAdvParamContainer::onModifyParLib(const QString &text)
{
  DEBTRACE("onModifyParLib " << text.toStdString());
  if (!_container) return;
  map<string,string> properties = _container->getProperties();
  _properties["parallelLib"] = text.toStdString();
  if (properties["parallelLib"] != text.toStdString())
    onModified();
}

void FormAdvParamContainer::onModifyIsMPI(bool isMpi)
{
  DEBTRACE("onModifyIsMPI " << isMpi);
  if (!_container) return;
  string text = "false";
  if (isMpi) text = "true";
  DEBTRACE(text);
  map<string,string> properties = _container->getProperties();
  _properties["isMPI"] = text;
  if (properties["isMPI"] != text)
    onModified();
}

void FormAdvParamContainer::onModifyMem(const QString &text)
{
  DEBTRACE("onModifyMem " << text.toStdString());
  if (!_container) return;
  if(_properties.count("name") && _properties["name"] != "")return; //do not modify resource parameter when specific resource is set
  map<string,string> properties = _container->getProperties();
  if(properties.count("mem_mb")==0 )properties["mem_mb"]="0"; //default value
  _properties["mem_mb"] = text.toStdString();
  if (properties["mem_mb"] != text.toStdString())
    onModified();
}

void FormAdvParamContainer::onModifyClock(const QString &text)
{
  DEBTRACE("onModifyClock " << text.toStdString());
  if (!_container) return;
  if(_properties.count("name") && _properties["name"] != "")return; //do not modify resource parameter when specific resource is set
  map<string,string> properties = _container->getProperties();
  if(properties.count("cpu_clock")==0 )properties["cpu_clock"]="0"; //default value
  _properties["cpu_clock"] = text.toStdString();
  if (properties["cpu_clock"] != text.toStdString())
    onModified();
}

void FormAdvParamContainer::onModifyNodes(const QString &text)
{
  DEBTRACE("onModifyNodes " << text.toStdString());
  if (!_container) return;
  if(_properties.count("name") && _properties["name"] != "")return; //do not modify resource parameter when specific resource is set
  map<string,string> properties = _container->getProperties();
  if(properties.count("nb_node")==0 )properties["nb_node"]="0"; //default value
  _properties["nb_node"] = text.toStdString();
  if (properties["nb_node"] != text.toStdString())
    onModified();
}

void FormAdvParamContainer::onModifyProcs(const QString &text)
{
  DEBTRACE("onModifyProcs " << text.toStdString());
  if (!_container) return;
  if(_properties.count("name") && _properties["name"] != "")return; //do not modify resource parameter when specific resource is set
  map<string,string> properties = _container->getProperties();
  if(properties.count("nb_proc_per_node")==0 )properties["nb_proc_per_node"]="0"; //default value
  _properties["nb_proc_per_node"] = text.toStdString();
  if (properties["nb_proc_per_node"] != text.toStdString())
    onModified();
}

void FormAdvParamContainer::onModifyCompos(const QString &text)
{
  DEBTRACE("onModifyCompo " << text.toStdString());
  if (!_container) return;
  map<string,string> properties = _container->getProperties();
  _properties["nb_component_nodes"] = text.toStdString();
  if (properties["nb_component_nodes"] != text.toStdString())
    onModified();
}

void FormAdvParamContainer::onModifyProcPar(const QString &text)
{
  DEBTRACE("onModifyProcPar "  << text.toStdString());
  if (!_container) return;
  map<string,string> properties = _container->getProperties();
  _properties["nb_parallel_procs"] = text.toStdString();
  if (properties["nb_parallel_procs"] != text.toStdString())
    onModified();
}

void FormAdvParamContainer::onModifyResourceName(const QString &text)
{
  DEBTRACE("onModifyResourceName "  << text.toStdString());
  if (!_container) return;
  map<string,string> properties = _container->getProperties();
  _properties["resource_name"] = text.toStdString();
  if (properties["resource_name"] != text.toStdString())
    onModified();
}

void FormAdvParamContainer::onModifyHostName(const QString &text)
{
  DEBTRACE("onModifyHostName "  << text.toStdString());
  if (!_container) return;
  if(_properties.count("name") && _properties["name"] != "")return; //do not modify resource parameter when specific resource is set

  map<string,string> properties = _container->getProperties();
  _properties["hostname"] = text.toStdString();
  if (properties["hostname"] != text.toStdString())
    onModified();
}

void FormAdvParamContainer::onModifyProcRes(const QString &text)
{
  DEBTRACE("onModifyProcRes "  << text.toStdString());
  if (!_container) return;
  if(_properties.count("name") && _properties["name"] != "")return; //do not modify resource parameter when specific resource is set
  map<string,string> properties = _container->getProperties();
  if(properties.count("nb_resource_procs")==0 )properties["nb_resource_procs"]="0"; //default value
  _properties["nb_resource_procs"] = text.toStdString();
  if (properties["nb_resource_procs"] != text.toStdString())
    onModified();
}

void FormAdvParamContainer::onModifyCompoList(const QString &text)
{
  DEBTRACE("onModifyCompoList "  << text.toStdString());
  if (!_container) return;
  if(_properties.count("name") && _properties["name"] != "")return; //do not modify resource parameter when specific resource is set
  map<string,string> properties = _container->getProperties();
  _properties["component_list"] = text.toStdString();
  if (properties["component_list"] != text.toStdString())
    onModified();
}

void FormAdvParamContainer::onModifyResourceList(const QString &text)
{
  DEBTRACE("onModifyResourceList "  << text.toStdString());
  if (!_container) return;
  if(_properties.count("name") && _properties["name"] != "")return; //do not modify resource parameter when specific resource is set
  map<string,string> properties = _container->getProperties();
  _properties["resource_list"] = text.toStdString();
  if (properties["resource_list"] != text.toStdString())
    onModified();
}
