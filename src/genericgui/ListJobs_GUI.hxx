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

#ifndef LISTJOBS_GUI_H
#define LISTJOBS_GUI_H

#include <QWidget>
#include "SALOME_NamingService.hxx"
#include "SALOME_Launcher.hh"

// Kind of column
// --------------

#define type_job   0
#define type_state 1
#define type_res   2
#define type_id    3

// Class for the dialog box
// ------------------------

class QPushButton;
class QTableWidget;
class QTableWidgetItem;
class QTimer;

namespace YACS
{

  namespace ENGINE
  {
    class Proc;
  }
  namespace HMI
  {
    class GenericGui;

    class BatchJobsListDialog: public QWidget {
      Q_OBJECT

    public:
      BatchJobsListDialog(QString title,GenericGui* genericGui);
      virtual ~BatchJobsListDialog();
      
    private:
      QWidget* build_buttons();
      void getSalomeLauncher();
      void filterJobFile();
      void filterDumpStateFile();

    private slots:
      void userOK();
      void userCancel();
      void userCell(QTableWidgetItem* cell);
      void build_table();
      void get_dump_file();

    protected:
      QTimer* _timer1, *_timer2;
      QPushButton* _ok;
      QTableWidget* _table;
      QString _jobFile;
      QString _dumpStateFile;
      QString _filteredJobFile;
      QString _filteredDumpStateFile;
      QString _hostname;
      long _id;
      Engines::SalomeLauncher_var _salome_launcher;
      GenericGui* _genericGui;
      YACS::ENGINE::Proc* _proc;
   };
  }
}

#endif // LISTJOBS_GUI_H
