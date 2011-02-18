#ifndef LISTJOBS_GUI_H
#define LISTJOBS_GUI_H

#include <QtGui/QtGui>
#include "SALOME_NamingService.hxx"
#include "SALOME_ContainerManager.hh"

// Kind of column
// --------------

#define type_job   0
#define type_state 1
#define type_res   2
#define type_id    3

// Class for the dialog box
// ------------------------


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
