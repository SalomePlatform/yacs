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

#ifndef __TOYNODE_HXX__
#define __TOYNODE_HXX__

#include "ServiceNode.hxx"
#include "AnyInputPort.hxx"
#include "OutputPort.hxx"
#include "InputPort.hxx"
#include "Any.hxx"

namespace YACS
{
  namespace ENGINE
  {
    class InputToyPort : public InputPort
    {
    public:
      InputToyPort(const InputToyPort& other, Node *newHelder);
      InputToyPort(const std::string& name, Node *node);
      void put(const void *data) throw(ConversionException);
      InputPort *clone(Node *newHelder) const;
      bool edIsManuallyInitialized() const;
      void *get() const throw(Exception);
      void edRemoveManInit();
      void put(Any *data);
      ~InputToyPort();
      Any *getAny() { return _data; }
      void exSaveInit();
      void exRestoreInit();
    protected:
      Any *_data;
      Any *_initData;
    };

    class OutputToyPort : public OutputPort
    {
    public:
      OutputToyPort(const std::string& name, Node *node, TypeCode *type);
      OutputToyPort(const OutputToyPort& other, Node *newHelder);
      void put(const void *data) throw(ConversionException);
      OutputPort *clone(Node *newHelder) const;
      void put(Any *data);
      ~OutputToyPort();
      void exInit();
      Any *get() { return _data; }
    protected:
      Any *_data;
    };

    class ToyNode : public ElementaryNode 
    {
    private:
      OutputToyPort _nbOfInputsPort;
    public:
      void execute();
      ToyNode(const ToyNode& other, ComposedNode *father);
      ToyNode(const std::string& name);
      OutputPort *edGetNbOfInputsOutputPort() { return &_nbOfInputsPort; }
      std::list<OutputPort *> getSetOfOutputPort() const;
      int getNumberOfOutputPorts()const;
      OutputPort *getOutputPort(const std::string& name) const throw(Exception);
    protected:
      Node *simpleClone(ComposedNode *father, bool editionOnly) const;
    public:
      static char NAME_FOR_NB[];
      static char MY_IMPL_NAME[];
    };

    class ToyNode1S : public ServiceNode
    {
    public:
      ToyNode1S(const std::string& name);
      ToyNode1S(const ToyNode1S& other, ComposedNode *father);
      void execute();
      std::string getKind() const;
      ServiceNode *createNode(const std::string& name);
    protected:
      Node *simpleClone(ComposedNode *father, bool editionOnly) const;
    public:
      static const char KIND[];
    };
    
    class ToyNode2S : public ServiceNode
    {
    public:
      ToyNode2S(const std::string& name);
      ToyNode2S(const ToyNode2S& other, ComposedNode *father);
      void execute();
      std::string getKind() const;
      ServiceNode *createNode(const std::string& name);
    protected:
      Node *simpleClone(ComposedNode *father, bool editionOnly) const;
    public:
      static const char KIND[];
    };

    class SeqToyNode : public ElementaryNode
    {
    private:
      OutputToyPort _seqOut;
      AnyInputPort _inIntValue;
    public:
      void execute();
      SeqToyNode(const SeqToyNode& other, ComposedNode *father);
      SeqToyNode(const std::string& name);
      OutputPort *edGetSeqOut() const { return (OutputPort *)&_seqOut; }
      InputPort *edGetInIntValue() const { return (InputPort *)&_inIntValue; }
      int getNumberOfInputPorts() const;
      std::list<InputPort *> getSetOfInputPort() const;
      InputPort *getInputPort(const std::string& name) const throw(Exception);
      int getNumberOfOutputPorts() const;
      std::list<OutputPort *> getSetOfOutputPort() const;
      OutputPort *getOutputPort(const std::string& name) const throw(Exception);
    protected:
      Node *simpleClone(ComposedNode *father, bool editionOnly) const;
    public:
      static char NAME_NBOFELTS_INSEQ_INPRT[];
      static char NAME_SEQ_OUTPRT[];
    };

    class Seq2ToyNode : public ElementaryNode
    {
      private:
      OutputToyPort _seqOut;
      AnyInputPort _inValue1;
      AnyInputPort _inValue2;
    public:
      void execute();
      Seq2ToyNode(const Seq2ToyNode& other, ComposedNode *father);
      Seq2ToyNode(const std::string& name);
      OutputToyPort *edGetSeqOut() const { return (OutputToyPort *)&_seqOut; }
      InputPort *edGetInValue1() const { return (InputPort *)&_inValue1; }
      InputPort *edGetInValue2() const { return (InputPort *)&_inValue2; }
      int getNumberOfInputPorts() const;
      std::list<InputPort *> getSetOfInputPort() const;
      InputPort *getInputPort(const std::string& name) const throw(Exception);
      int getNumberOfOutputPorts() const;
      std::list<OutputPort *> getSetOfOutputPort() const;
      OutputPort *getOutputPort(const std::string& name) const throw(Exception);
    protected:
      Node *simpleClone(ComposedNode *father, bool editionOnly) const;
    public:
      static char NAME_SEQ_INPRT1[];
      static char NAME_SEQ_INPRT2[];
      static char NAME_SEQ_OUTPRT[];
    };

    class Seq3ToyNode : public ElementaryNode
    {
      private:
      OutputToyPort _seqOut;
      AnyInputPort _inValue1;
      AnyInputPort _inValue2;
    public:
      void execute();
      Seq3ToyNode(const Seq3ToyNode& other, ComposedNode *father);
      Seq3ToyNode(const std::string& name);
      OutputToyPort *edGetSeqOut() const { return (OutputToyPort *)&_seqOut; }
      InputPort *edGetInValue1() const { return (InputPort *)&_inValue1; }
      InputPort *edGetInValue2() const { return (InputPort *)&_inValue2; }
      int getNumberOfInputPorts() const;
      std::list<InputPort *> getSetOfInputPort() const;
      InputPort *getInputPort(const std::string& name) const throw(Exception);
      int getNumberOfOutputPorts() const;
      std::list<OutputPort *> getSetOfOutputPort() const;
      OutputPort *getOutputPort(const std::string& name) const throw(Exception);
    protected:
      Node *simpleClone(ComposedNode *father, bool editionOnly) const;
    public:
      static char NAME_SEQ_INPRT1[];
      static char NAME_SEQ_INPRT2[];
      static char NAME_SEQ_OUTPRT[];
    };
    
    class LimitNode;

    class InputLimitPort : public InputPort
    {
      friend class LimitNode;
    public:
      void put(const void *data) throw(ConversionException);
      InputPort *clone(Node *newHelder) const;
      bool edIsManuallyInitialized() const;
      void *get() const throw(Exception);
      void edRemoveManInit();
      void put(Any *data);
      ~InputLimitPort();
      Any *getAny() { return _data; }
      void exSaveInit();
      void exRestoreInit();
    private:
      InputLimitPort(const InputLimitPort& other, Node *newHelder);
      InputLimitPort(const std::string& name, Node *node);
    protected:
      Any *_data;
      Any *_initData;
    };

    class OutputLimitPort : public OutputPort
    {
      friend class LimitNode;
    public:
      void put(const void *data) throw(ConversionException);
      OutputPort *clone(Node *newHelder) const;
      void put(Any *data);
      ~OutputLimitPort();
      Any *get() { return _data; }
    private:
      OutputLimitPort(const OutputLimitPort& other, Node *newHelder);
      OutputLimitPort(const std::string& name, Node *node, TypeCode *type);
    protected:
      Any *_data;
    };

    class LimitNode : public ElementaryNode
    {
    private:
      double _limit;
      double _current;
      InputLimitPort _entry;
      OutputLimitPort _switchPort;
      OutputLimitPort _counterPort;
    public:
      void init(bool start=true);
      void execute();
      void setLimit(double limit) { _limit=limit; }
      InputPort *getEntry() { return &_entry; }
      OutputPort *getSwitchPort() { return &_switchPort; }
      OutputPort *getCounterPort() { return &_counterPort; }
      std::list<InputPort *> getSetOfInputPort() const;
      std::list<OutputPort *> getSetOfOutputPort() const;
      InputPort *getInputPort(const std::string& name) const throw(Exception);
      OutputPort *getOutputPort(const std::string& name) const throw(Exception);
      LimitNode(const LimitNode& other, ComposedNode *father);
      LimitNode(const std::string& name);
    protected:
      Node *simpleClone(ComposedNode *father, bool editionOnly) const;
    public:
      static char MY_IMPL_NAME[];
      static char NAME_FOR_SWPORT[];
      static char NAME_FOR_SWPORT2[];
    };
  }
}

#endif
