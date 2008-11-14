#ifndef _TABLEPORTSEDITION_HXX_
#define _TABLEPORTSEDITION_HXX_

#include "ui_TablePorts.h"

#include <QDialog>
#include <QTreeWidget>

namespace YACS
{
  namespace HMI
  {
    class SubjectElementaryNode;

    class TablePortsEdition: public QWidget, public Ui::fm_TablePorts
    {
      Q_OBJECT

    public:
      TablePortsEdition(bool inPorts, QWidget *parent = 0);
      virtual ~TablePortsEdition();
      void setNode(SubjectElementaryNode* node);
      virtual void setEditablePorts(bool isEditable);
      virtual void adjustColumns();

    public slots:
      void on_pb_up_clicked();
      void on_pb_down_clicked();
      void on_pb_insert_clicked();
      void on_pb_add_clicked();
      void on_pb_remove_clicked();
      void on_cb_insert_activated(const QString& text);
            
    protected:
      bool _inPorts;
      SubjectElementaryNode* _node;
    private:
    };

  }
}


#endif
