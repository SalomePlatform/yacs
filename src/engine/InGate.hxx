#ifndef __INGATE_HXX__
#define __INGATE_HXX__

#include "Port.hxx"
#include "define.hxx"

namespace YACS
{
  namespace ENGINE
  {
    class InGate : public Port
    {
      friend class Bloc;
      friend class Node;
    protected:
      int _nbPrecursor;
      int _nbPrecursorDone;
      static const char NAME[];
    private:
      //for graphs algs
      mutable Colour _colour;
    public:
      InGate(Node *node);
      std::string getNameOfTypeOfCurrentInstance() const;
      void exNotifyFromPrecursor();
      void edAppendPrecursor();
      void edRemovePrecursor();
      void edSet(int nbOfPrecursors);
      void exReset();
      bool exIsReady() const;
    private:
      void initForDFS() const { _colour=YACS::White; }
    };
  }
}

#endif
