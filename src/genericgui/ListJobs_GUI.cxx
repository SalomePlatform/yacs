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

#include <iostream>
#include <fstream>

#include "ListJobs_GUI.hxx"
#include "RuntimeSALOME.hxx"
#include "GenericGui.hxx"
#include "QtGuiContext.hxx"
#include "YACSGuiLoader.hxx"
#include "Logger.hxx"
#include "YacsTrace.hxx"
#include "LoadState.hxx"

#include <QBoxLayout>
#include <QFileInfo>
#include <QMessageBox>
#include <QPushButton>
#include <QTableWidget>
#include <QTimer>

using namespace std;
using namespace YACS::HMI;
using namespace YACS::ENGINE;

BatchJobsListDialog::BatchJobsListDialog(QString title,GenericGui* genericGui): QWidget(),_genericGui(genericGui),_proc(NULL) {

  setWindowTitle(title);

  // Connection to Salome Launcher
  getSalomeLauncher();

  // Add a layout
  QBoxLayout* box = new QBoxLayout(QBoxLayout::TopToBottom, this);
  setLayout(box);

  _table = new QTableWidget(this);
  _table->setColumnCount(4);

  build_table();

  // Add table
  box->addWidget(_table);

  // Add buttons
  QWidget* buttons = build_buttons();
  box->addWidget(buttons);

  // No line selected
  _ok->setEnabled(false);
  _id = -1;

  // Set timer to refresh table
  _timer1 = new QTimer(this);
  connect(_timer1, SIGNAL(timeout()), this, SLOT(build_table()));
  _timer1->start(30000);

}

BatchJobsListDialog::~BatchJobsListDialog(){
}

void BatchJobsListDialog::getSalomeLauncher(){
  YACS::ENGINE::RuntimeSALOME* runTime = YACS::ENGINE::getSALOMERuntime();
  CORBA::ORB_ptr orb = runTime->getOrb();
  SALOME_NamingService* NS = new SALOME_NamingService(orb);
  CORBA::Object_var obj = NS->Resolve("/SalomeLauncher");
  _salome_launcher = Engines::SalomeLauncher::_narrow(obj);
  if (CORBA::is_nil(_salome_launcher))
    throw YACS::Exception("Salome Launcher not reachable!!");
}

// Display list of current batch jobs and allow selection only on running jobs
void BatchJobsListDialog::build_table() {

  int irow = 0;

  _table->clear();

  Engines::JobsList* joblist = _salome_launcher->getJobsList();

  int nblines = joblist->length();
  _table->setRowCount(nblines);

  QStringList titres;
  titres << "Job Name" << "State" << "Resource" << "Launcher Id";
  _table->setHorizontalHeaderLabels(titres);

  for(int i=0;i<nblines;i++){
    Engines::JobDescription descr = (*joblist)[i];
    string jobType = CORBA::string_dup(descr.job_parameters.job_type);
    // We display only jobs of type yacs_file
    if(jobType.find("yacs_file")!=string::npos){
      long jobId = descr.job_id;
      ostringstream sstId;
      sstId << jobId;
      string jobName = CORBA::string_dup(descr.job_parameters.job_name);
      string jobState = _salome_launcher->getJobState(jobId);
      string jobMachine = CORBA::string_dup(descr.job_parameters.resource_required.name);
      QTableWidgetItem* item0 = new QTableWidgetItem(jobName.c_str()    , type_job);
      QTableWidgetItem* item1 = new QTableWidgetItem(jobState.c_str()   , type_state);
      QTableWidgetItem* item2 = new QTableWidgetItem(jobMachine.c_str() , type_res  );
      QTableWidgetItem* item3 = new QTableWidgetItem(sstId.str().c_str(), type_id   );
      _table->setItem(irow, 0, item0);
      _table->setItem(irow, 1, item1);
      _table->setItem(irow, 2, item2);
      _table->setItem(irow, 3, item3);
      if(jobState != "RUNNING"){
        item0->setFlags(Qt::NoItemFlags);
        item1->setFlags(Qt::NoItemFlags);
        item2->setFlags(Qt::NoItemFlags);
        item3->setFlags(Qt::NoItemFlags);
      }
      else if(jobId == _id){
        item0->setSelected(true);
        item1->setSelected(true);
        item2->setSelected(true);
        item3->setSelected(true);
      }
      irow++;
    }
  }
    
  connect(_table, SIGNAL( itemClicked(QTableWidgetItem*) ), this, SLOT( userCell(QTableWidgetItem*) ));

}

// Build buttons in a layout
// -------------------------

QWidget* BatchJobsListDialog::build_buttons() {
  _ok     = new QPushButton("OK"    , this);
  QPushButton* cancel = new QPushButton("Cancel", this);

  connect(_ok    , SIGNAL(clicked()), this, SLOT(userOK    ()));
  connect(cancel, SIGNAL(clicked()), this, SLOT(userCancel()));

  QBoxLayout* layout = new QBoxLayout(QBoxLayout::LeftToRight, this);

  layout->addWidget(_ok);
  layout->addWidget(cancel);

  QWidget* buttons = new QWidget(this);
  buttons->setLayout(layout);

  return(buttons);
}

// Click on a cell button
// ----------------------

void BatchJobsListDialog::userCell(QTableWidgetItem* cell) {
  if ( cell->flags() ) {
    int line = cell->row();
    string sid = _table->item(line,3)->text().toStdString();
    _id = atoi(sid.c_str());
    _ok->setEnabled(true);

    QTableWidget* table = cell->tableWidget();
    for (int c=0; c<4; c++) {
      QTableWidgetItem* it = table->item(line, c);
      it->setSelected(true);
    };
  };
}

// Click on OK button
// ------------------

void BatchJobsListDialog::userOK() {

  // get job file name
  Engines::JobParameters* jobParam = _salome_launcher->getJobParameters(_id);
  _jobFile = CORBA::string_dup(jobParam->job_file);
  _dumpStateFile = QString("/tmp/%1/dumpState_%2.xml").arg(getenv("USER")).arg(QFileInfo(_jobFile).baseName());

  // stop first timer and hide window
  _timer1->stop();
  hide();

  // replace objref by string in YACS schema job file to avoid trying to reach remote objects
  filterJobFile();

  // display yacs graph
  YACSGuiLoader *loader = _genericGui->getLoader();
  _proc = loader->load(_filteredJobFile.toLatin1());
  if (!_proc) {
    QMessageBox msgBox(QMessageBox::Critical, "Import Batch Schema, native YACS XML format",
                       "The xml graph has not the native YACS XML format or is not readable." );
    msgBox.exec();
  } 
  else {
    YACS::ENGINE::Logger* logger= _proc->getLogger("parser");
    if(!logger->isEmpty()) {
      DEBTRACE(logger->getStr());
    };
    _genericGui->createContext(_proc, _filteredJobFile, _filteredJobFile, false);
  };

  // start second timer to get remote graph state xml file
  _timer2 = new QTimer(this);
  connect(_timer2, SIGNAL(timeout()), this, SLOT(get_dump_file()));
  _timer2->start(30000);
  get_dump_file();
}

// Click on CANCEL button
// ----------------------

void BatchJobsListDialog::userCancel() {
  _timer1->stop();
  hide();
}

// get remote graph state xml file

void BatchJobsListDialog::get_dump_file()
{
  int execState = YACS::NOTYETINITIALIZED;
  // get batch job state
  string jobState = _salome_launcher->getJobState(_id);
  // get dump state remote file
  bool ret = _salome_launcher->getJobDumpState(_id, QFileInfo(_dumpStateFile).absolutePath().toLatin1().constData());
  if(ret){
    // replace objref by string in dump state file
    filterDumpStateFile();
    // parse dump state file and load proc
    YACS::ENGINE::loadState(_proc,_filteredDumpStateFile.toStdString());
    // display remote graph states
    GuiExecutor *executor = QtGuiContext::getQtCurrent()->getGuiExecutor();
    execState = executor->updateSchema(jobState);
  }
  // stop timer if job is not running
  if((execState!=YACS::RUNNING)&&(execState!=YACS::NOTYETINITIALIZED))
    _timer2->stop();
}

// filtering of _jobFile to replace objref by string
void BatchJobsListDialog::filterJobFile()
{
  _filteredJobFile = QString("/tmp/%1/%2.xml").arg(getenv("USER")).arg(QFileInfo(_jobFile).baseName());
  // reading input file
  ifstream infile(_jobFile.toStdString().c_str());
  if(!infile){
    string errmsg = "File " + _jobFile.toStdString() + " doesn't exist!!";
    throw YACS::Exception(errmsg);
  }
  string buffer;
  vector<string> objref;
  while( !infile.eof() ){
    getline(infile,buffer);
    // look for objref and memorize them in vector
    if( (buffer.find("objref") != string::npos) && (buffer.find("IDL") != string::npos) ){
      size_t pos1 = buffer.find("\"");
      size_t pos2 = buffer.find("\"",pos1+1);
      objref.push_back(buffer.substr(pos1+1,pos2-pos1-1));
    }
  }
  infile.close();
  // reread the input file
  infile.open(_jobFile.toStdString().c_str());
  // open the output file
  ofstream outfile(_filteredJobFile.toStdString().c_str());
  if(!outfile){
    string errmsg = "Impossible to create the file " + _filteredJobFile.toStdString() + "!!";
    throw YACS::Exception(errmsg);
  }
  while( !infile.eof() ){
    getline(infile,buffer);
    // replace objref by string
    if( ((buffer.find("objref") == string::npos) || (buffer.find("IDL") == string::npos)) && (buffer.find("/objref") == string::npos) ){
      string tmp = buffer;
      for(int i=0;i<objref.size();i++){
        size_t pos = buffer.find(objref[i]);
        if(pos != string::npos)
          tmp = buffer.substr(0,pos) + "string" + buffer.substr(pos+objref[i].size());
      }
      outfile << tmp << endl;
    }
  }
}

// filtering of _dumpStateFile to replace objref by string
void BatchJobsListDialog::filterDumpStateFile()
{
  string buffer;
  _filteredDumpStateFile = QString("/tmp/%1/filtered_%2.xml").arg(getenv("USER")).arg(QFileInfo(_dumpStateFile).baseName());
  ifstream infile(_dumpStateFile.toStdString().c_str());
  if(!infile){
    string errmsg = "File " + _dumpStateFile.toStdString() + " doesn't exist!!";
    throw YACS::Exception(errmsg);
  }
  ofstream outfile(_filteredDumpStateFile.toStdString().c_str());
  if(!outfile){
    string errmsg = "Impossible to create the file " + _filteredDumpStateFile.toStdString() + "!!";
    throw YACS::Exception(errmsg);
  }
  // replace objref by string in dump state file
  while( !infile.eof() ){
    getline(infile,buffer);
    size_t pos = buffer.find("objref");
    while(pos != string::npos){
      buffer.replace(pos,6,"string");
      pos = buffer.find("objref");
    }
    outfile << buffer << endl;
  }
}

