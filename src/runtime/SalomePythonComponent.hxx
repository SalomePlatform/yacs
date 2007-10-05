#ifndef __SALOMEPYTHONCOMPONENT_HXX__
#define __SALOMEPYTHONCOMPONENT_HXX__

#include "ComponentInstance.hxx"

namespace YACS
{
  namespace ENGINE
  {
    class SalomePythonComponent : public ComponentInstance
    {
    public:
      SalomePythonComponent(const std::string &name);
      SalomePythonComponent(const SalomePythonComponent& other);
      std::string getPlacementId() const;
      virtual ~SalomePythonComponent();
      virtual void load();
      virtual void unload();
      virtual bool isLoaded();
      virtual std::string getKind() const;
      virtual ComponentInstance* clone() const;
      virtual std::string getFileRepr() const;
      virtual ServiceNode *createNode(const std::string &name);
      //! The specific method that justified SalomePythonComponent class.
      std::string getStringValueToExportInInterp() const;
    public:
      unsigned _cntForRepr;
      static unsigned _cntForReprS;
      static const char KIND[];
    };
  }
}

#endif
