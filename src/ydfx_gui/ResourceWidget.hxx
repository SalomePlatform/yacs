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
