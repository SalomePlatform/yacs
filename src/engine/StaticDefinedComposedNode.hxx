#ifndef __STATICDEFINEDCOMPOSEDNODE_HXX__
#define __STATICDEFINEDCOMPOSEDNODE_HXX__

#include "ComposedNode.hxx"

namespace YACS
{
  namespace ENGINE
  {
    /*!
     * Abstract class, that factorizes all the treatments relative to resource management for ComposedNode that have
     * their connectivity fully defined before launching ; which is not always the case for classes inheriting from DynParaLoop.
     */
    class StaticDefinedComposedNode : public ComposedNode
    {
    protected:
      StaticDefinedComposedNode(const std::string& name);
      StaticDefinedComposedNode(const StaticDefinedComposedNode& other, ComposedNode *father);
    public:
      bool isPlacementPredictableB4Run() const;
      bool isMultiplicitySpecified(unsigned& value) const;
      void forceMultiplicity(unsigned value);
    };
  }
}

#endif
