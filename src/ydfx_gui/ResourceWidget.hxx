// Copyright (C) 2017-2023  CEA, EDF
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

#ifndef RESOURCEWIDGET_HXX
#define RESOURCEWIDGET_HXX

#include <QtWidgets>
#include "AbstractResourceModel.hxx"

class YDFXWIDGETS_EXPORT ResourceWidget: public QScrollArea
{
  Q_OBJECT
  public:
    ResourceWidget(AbstractResourceModel* model, QWidget* parent=0);
    virtual ~ResourceWidget();

  public slots:
    virtual void updateParallelizeStatus(bool v);
    virtual void updateWantedMachine( const QString& v);
    virtual void updateNbprocs(int v);
    virtual void updateRemoteDir(const QString &v);
    virtual void updateLocalDir(const QString &v);
    virtual void updateWckey(const QString &v);
    virtual void updateMaxHours(int v);
    virtual void updateMaxMinutes(int v);
    virtual void onChoseLocaldir();
    virtual void inputSelectionChanged();
    virtual void addInputFiles();
    virtual void removeInputFiles();
  private:
    QWidget * createClusterWidgets();
  private:
    AbstractResourceModel* _model;
    QWidget * _clusterBox;
    QLineEdit * _localdirEdit;
    QLineEdit * _remotedirEdit;
    QListWidget * _inputFilesList;
    QPushButton *_removeInputFilesButton;
};

#endif // RESOURCEWIDGET_HXX
