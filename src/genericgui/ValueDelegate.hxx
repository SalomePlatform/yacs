#ifndef _VALUEDELEGATE_HXX
#define _VALUEDELEGATE_HXX


#include <QItemDelegate>
#include <QModelIndex>
#include <QObject>
#include <QLineEdit>
#include <QSpinBox>
#include <QVariant>
#include <set>

namespace YACS
{
  namespace HMI
  {
    class Subject;
    class ValueDelegate;

    class GenericEditor
    {
    public:
      GenericEditor();
      virtual ~GenericEditor();
      virtual void setSubject(Subject* subject);
      virtual void setColumn(int column);
      virtual void setDelegate(const ValueDelegate* delegate);
      virtual QString GetStrValue();
      virtual Subject* getSubject();
      virtual int getColumnInSubject();
      virtual void setData(QVariant val);
      virtual bool firstSetData();
    protected:
      Subject* _subject;
      int _column;
      const ValueDelegate* _delegate;
      bool _first;
    };

    class GeneralEditor: public QLineEdit, public GenericEditor
    {
      Q_OBJECT
      
    public:
      GeneralEditor(Subject* subject,
                    const ValueDelegate* delegate,
                    int column = 0,
                    QWidget* parent = 0);
      virtual ~GeneralEditor();
      virtual QString GetStrValue();
      virtual void setData(QVariant val);
    };

    class IntEditor: public QSpinBox, public GenericEditor
    {
      Q_OBJECT
      
    public:
      IntEditor(Subject* subject,
                const ValueDelegate* delegate,
                int column = 0,
                QWidget* parent = 0);
      virtual ~IntEditor();
      virtual QString GetStrValue();
      virtual void setData(QVariant val);
    };



    class ValueDelegate : public QItemDelegate
    {
      Q_OBJECT
      
    public:
      ValueDelegate(QObject *parent = 0);
      virtual ~ValueDelegate();
      
      virtual QWidget *createEditor(QWidget *parent,
                                    const QStyleOptionViewItem &option,
                                    const QModelIndex &index) const;
      
      virtual void setEditorData(QWidget *editor,
                                 const QModelIndex &index) const;
      virtual void setModelData(QWidget *editor,
                                QAbstractItemModel *model,
                                const QModelIndex &index) const;
      
      virtual void updateEditorGeometry(QWidget *editor,
                                        const QStyleOptionViewItem &option,
                                        const QModelIndex &index) const;
      virtual void setResultEditing(QWidget *editor, bool isOk);

    protected:
      mutable std::map<Subject*, std::string> _errorMap;

    };
  }
}
#endif
