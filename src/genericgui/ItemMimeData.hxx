#ifndef _ITEMMIMEDATA_HXX_
#define _ITEMMIMEDATA_HXX_

#include <QMimeData>
#include <string>

namespace YACS
{
  namespace ENGINE
  {
    class Catalog;
  }

  namespace HMI
  {
    class Subject;

    /*!
     * ItemMimeData is used for Drag and Drop of Scene Items.
     */
    class ItemMimeData: public QMimeData
    {
    public:
      ItemMimeData();
      virtual ~ItemMimeData();
      virtual void setSubject(Subject *sub);
      virtual void setCatalog(YACS::ENGINE::Catalog *cata);
      virtual void setCataName(std::string cataName);
      virtual void setCompo(std::string compo);
      virtual void setType(std::string aType);
      virtual void setCase(int aCase);
      virtual Subject* getSubject() const;
      virtual YACS::ENGINE::Catalog* getCatalog() const;
      virtual std::string getCataName() const;
      virtual std::string getCompo() const;
      virtual std::string getType() const;
      virtual int getCase() const;
    protected:
      Subject *_sub;
      YACS::ENGINE::Catalog *_catalog;
      std::string _cataName;
      std::string _compoName;
      std::string _typeName;
      int _swCase;
    };
    
  }
}

#endif
