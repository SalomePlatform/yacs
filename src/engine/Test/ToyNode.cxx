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

#include "ToyNode.hxx"
#include "TypeCode.hxx"
#include <iostream>

using namespace YACS::ENGINE;
using namespace std;

char ToyNode::MY_IMPL_NAME[]="TOY";

const char ToyNode1S::KIND[]="TESTKIND1";

const char ToyNode2S::KIND[]="TESTKIND2";

char LimitNode::MY_IMPL_NAME[]="LIMIT";

char ToyNode::NAME_FOR_NB[]="NbOfEntries";

char SeqToyNode::NAME_NBOFELTS_INSEQ_INPRT[]="NbOfEltsInSeq";

char SeqToyNode::NAME_SEQ_OUTPRT[]="MySeq";

char Seq2ToyNode::NAME_SEQ_INPRT1[]="SeqIn1";

char Seq2ToyNode::NAME_SEQ_INPRT2[]="SeqIn2";

char Seq2ToyNode::NAME_SEQ_OUTPRT[]="SeqOut";

char Seq3ToyNode::NAME_SEQ_INPRT1[]="SeqIn1";

char Seq3ToyNode::NAME_SEQ_INPRT2[]="SeqIn2";

char Seq3ToyNode::NAME_SEQ_OUTPRT[]="SeqOut";

char LimitNode::NAME_FOR_SWPORT[]="SwitchPort";

char LimitNode::NAME_FOR_SWPORT2[]="SwitchPort2";

InputToyPort::InputToyPort(const InputToyPort& other, Node *newHelder):InputPort(other,newHelder),DataPort(other,newHelder),Port(other,newHelder),
                                                                       _data(0),_initData(0)
{
  if(other._data)
    _data=other._data->clone();
  if(other._initData)
    _initData=other._initData->clone();
}

InputToyPort::InputToyPort(const string& name, Node *node):InputPort(name, node, Runtime::_tc_double),DataPort(name, node, Runtime::_tc_double),Port(node),_data(0),_initData(0)
{
}

void InputToyPort::put(const void *data) throw(ConversionException)
{
  put((Any *)data);
}

bool InputToyPort::edIsManuallyInitialized() const
{
  return _initData!=0;
}

void *InputToyPort::get() const throw(YACS::Exception)
{
  return (void *)_data;
}

void InputToyPort::edRemoveManInit()
{
  if(_initData)
    _initData->decrRef();
  _initData=0;
  InputPort::edRemoveManInit();
}

InputPort *InputToyPort::clone(Node *newHelder) const
{
  return new InputToyPort(*this,newHelder);
}

void InputToyPort::put(Any *data)
{
  if(_data)
    _data->decrRef();
  _data=data;
  _data->incrRef();
}

void InputToyPort::exSaveInit()
  {
    if(_initData) _initData->decrRef();
    _initData=_data;
    _initData->incrRef();
  }

void InputToyPort::exRestoreInit()
  {
    if(!_initData)return;
    if(_data) _data->decrRef();
    _data=_initData;
    _data->incrRef();
  }

InputToyPort::~InputToyPort()
{
  if(_data)
    _data->decrRef();
  if(_initData)
    _initData->decrRef();
}

OutputToyPort::OutputToyPort(const string& name, Node *node, TypeCode *type):OutputPort(name, node, type),DataPort(name, node, type),Port(node),_data(0)
{
}

OutputToyPort::OutputToyPort(const OutputToyPort& other, Node *newHelder):OutputPort(other,newHelder),DataPort(other,newHelder),Port(other,newHelder),_data(0)
{
  if(other._data)
    _data=other._data->clone();
}

void OutputToyPort::put(const void *data) throw(ConversionException)
{
  put((Any *)data);
  OutputPort::put(data);
}

OutputPort *OutputToyPort::clone(Node *newHelder) const
{
  return new OutputToyPort(*this,newHelder);
}

void OutputToyPort::put(Any *data)
{
  if(_data)
    _data->decrRef();
  _data=data;
  _data->incrRef();
}

OutputToyPort::~OutputToyPort()
{
  if(_data)
    _data->decrRef();
}

void OutputToyPort::exInit()
{
  if(_data)
    {
      _data->decrRef();
      _data=0;
    }
}

ToyNode::ToyNode(const ToyNode& other, ComposedNode *father):ElementaryNode(other,father),_nbOfInputsPort(other._nbOfInputsPort,this)
{
}

ToyNode::ToyNode(const string& name):ElementaryNode(name),_nbOfInputsPort(NAME_FOR_NB,this,Runtime::_tc_int)
{
  _implementation=MY_IMPL_NAME;
}

void ToyNode::execute()
{
  int i=0;
  double d=0.;
  for(list<InputPort *>::iterator iter=_setOfInputPort.begin();iter!=_setOfInputPort.end();iter++)
    {
      i++;
      InputToyPort *p=(InputToyPort *) (*iter);
      d+=p->getAny()->getDoubleValue();
    }
  int size=_setOfOutputPort.size();
  if(size)
    d/=(double)(size);
  Any *tmp=AtomAny::New((int)_setOfInputPort.size());
  _nbOfInputsPort.put((const void *)tmp);
  tmp->decrRef();
  for(list<OutputPort *>::iterator iter2=_setOfOutputPort.begin();iter2!=_setOfOutputPort.end();iter2++)
    {
      Any *tmp=AtomAny::New(d);
      (*iter2)->put(tmp);
      tmp->decrRef();
    }
}

std::list<OutputPort *> ToyNode::getSetOfOutputPort() const
{
  list<OutputPort *> ret=ElementaryNode::getSetOfOutputPort();
  ret.push_back((OutputPort *)&_nbOfInputsPort);
  return ret;
}

int ToyNode::getNumberOfOutputPorts() const
{
  return ElementaryNode::getNumberOfInputPorts()+1;
}

OutputPort *ToyNode::getOutputPort(const std::string& name) const throw(YACS::Exception)
{
  if(name==NAME_FOR_NB)
    return (OutputPort *)&_nbOfInputsPort;
  else
    return ElementaryNode::getOutputPort(name);
}

Node *ToyNode::simpleClone(ComposedNode *father, bool editionOnly) const
{
  return new ToyNode(*this,father);
}

ToyNode1S::ToyNode1S(const std::string& name):ServiceNode(name)
{
}

ToyNode1S::ToyNode1S(const ToyNode1S& other, ComposedNode *father):ServiceNode(other,father)
{
}

void ToyNode1S::execute()
{
  //nothing only to test deployment calculation not for running.
}

std::string ToyNode1S::getKind() const
{
  return KIND;
}

ServiceNode *ToyNode1S::createNode(const std::string& name)
{
  ToyNode1S* node=new ToyNode1S(name);
  node->setComponent(_component);
  return node;
}

Node *ToyNode1S::simpleClone(ComposedNode *father, bool editionOnly) const
{
  return new ToyNode1S(*this,father);
}

ToyNode2S::ToyNode2S(const std::string& name):ServiceNode(name)
{
}

ToyNode2S::ToyNode2S(const ToyNode2S& other, ComposedNode *father):ServiceNode(other,father)
{
}

void ToyNode2S::execute()
{
  //nothing only to test deployment calculation not for running.
}

std::string ToyNode2S::getKind() const
{
  return KIND;
}

ServiceNode *ToyNode2S::createNode(const std::string& name)
{
  ToyNode2S* node=new ToyNode2S(name);
  node->setComponent(_component);
  return node;
}

Node *ToyNode2S::simpleClone(ComposedNode *father, bool editionOnly) const
{
  return new ToyNode2S(*this,father);
}

void SeqToyNode::execute()
{
  int val=_inIntValue.getIntValue();
  SequenceAny *tmp=SequenceAny::New(Runtime::_tc_double,val);
  double d=(double) val;
  for(int i=0;i<val;i++)
    {
      Any *tmp2=AtomAny::New(d);
      tmp->setEltAtRank(i,tmp2);
      tmp2->decrRef();
      d+=1.;
    }
  _seqOut.put((const void *)tmp);
  tmp->decrRef();
}

SeqToyNode::SeqToyNode(const SeqToyNode& other, ComposedNode *father):ElementaryNode(other,father),
                                                                      _seqOut(other._seqOut,this),
                                                                      _inIntValue(other._inIntValue,this)
{
}

SeqToyNode::SeqToyNode(const std::string& name):ElementaryNode(name),_seqOut(NAME_SEQ_OUTPRT,this,Runtime::_tc_double),
                                                _inIntValue(NAME_NBOFELTS_INSEQ_INPRT,this,Runtime::_tc_int)
{
  _implementation=ToyNode::MY_IMPL_NAME;
  TypeCodeSeq *tc=new TypeCodeSeq("","",Runtime::_tc_double);
  _seqOut.edSetType(tc);
  tc->decrRef();
}

int SeqToyNode::getNumberOfInputPorts() const
{
  return ElementaryNode::getNumberOfInputPorts()+1;
}

std::list<InputPort *> SeqToyNode::getSetOfInputPort() const
{
  list<InputPort *> ret=ElementaryNode::getSetOfInputPort();
  ret.push_back((InputPort *)&_inIntValue);
  return ret;
}

InputPort *SeqToyNode::getInputPort(const std::string& name) const throw(YACS::Exception)
{
  if(name==NAME_NBOFELTS_INSEQ_INPRT)
    return (InputPort *)&_inIntValue;
  else
    return ElementaryNode::getInputPort(name);
}

int SeqToyNode::getNumberOfOutputPorts() const
{
  return ElementaryNode::getNumberOfOutputPorts()+1;
}

std::list<OutputPort *> SeqToyNode::getSetOfOutputPort() const
{
  list<OutputPort *> ret=ElementaryNode::getSetOfOutputPort();
  ret.push_back((OutputPort *)&_seqOut);
  return ret;
}

OutputPort *SeqToyNode::getOutputPort(const std::string& name) const throw(YACS::Exception)
{
  if(name==NAME_SEQ_OUTPRT)
    return (OutputPort *)&_seqOut;
  else
    return ElementaryNode::getOutputPort(name);
}

Node *SeqToyNode::simpleClone(ComposedNode *father, bool editionOnly) const
{
  return new SeqToyNode(*this,father);
}

void Seq2ToyNode::execute()
{
  SequenceAny *vals1=(SequenceAny *)_inValue1.getValue();
  SequenceAny *vals2=(SequenceAny *)_inValue2.getValue();
  int val=vals1->size();
  SequenceAny *tmp=SequenceAny::New(Runtime::_tc_int,val);
  for(int i=0;i<val;i++)
    {
      Any *tmp2=AtomAny::New((int)((*vals1)[i]->getDoubleValue()+(*vals2)[i]->getDoubleValue()));
      tmp->setEltAtRank(i,tmp2);
      tmp2->decrRef();
    }
  _seqOut.put((const void *)tmp);
  tmp->decrRef();
}

Seq2ToyNode::Seq2ToyNode(const Seq2ToyNode& other, ComposedNode *father):ElementaryNode(other,father),
                                                                        _seqOut(other._seqOut,this),
                                                                        _inValue1(other._inValue1,this),
                                                                        _inValue2(other._inValue1,this)
{
}

Seq2ToyNode::Seq2ToyNode(const std::string& name):ElementaryNode(name),_seqOut(NAME_SEQ_OUTPRT,this,Runtime::_tc_double),
                                                  _inValue1(NAME_SEQ_INPRT1,this,Runtime::_tc_int),
                                                  _inValue2(NAME_SEQ_INPRT2,this,Runtime::_tc_int)
{
  _implementation=ToyNode::MY_IMPL_NAME;
  TypeCodeSeq *tc=new TypeCodeSeq("","",Runtime::_tc_double);
  _inValue1.edSetType(tc);
  _inValue2.edSetType(tc);
  tc->decrRef();
  tc=new TypeCodeSeq("","",Runtime::_tc_int);
  _seqOut.edSetType(tc);
  tc->decrRef();
}

int Seq2ToyNode::getNumberOfInputPorts() const
{
  return ElementaryNode::getNumberOfOutputPorts()+2;
}

std::list<InputPort *> Seq2ToyNode::getSetOfInputPort() const
{
  list<InputPort *> ret=ElementaryNode::getSetOfInputPort();
  ret.push_back((InputPort *)&_inValue1);
  ret.push_back((InputPort *)&_inValue2);
  return ret;
}

InputPort *Seq2ToyNode::getInputPort(const std::string& name) const throw(YACS::Exception)
{
  if(name==NAME_SEQ_INPRT1)
    return (InputPort *)&_inValue1;
  else if(name==NAME_SEQ_INPRT2)
    return (InputPort *)&_inValue2;
  else
    return ElementaryNode::getInputPort(name);
}

int Seq2ToyNode::getNumberOfOutputPorts() const
{
  return ElementaryNode::getNumberOfOutputPorts()+1;
}

std::list<OutputPort *> Seq2ToyNode::getSetOfOutputPort() const
{
  list<OutputPort *> ret=ElementaryNode::getSetOfOutputPort();
  ret.push_back((OutputPort *)&_seqOut);
  return ret;
}

OutputPort *Seq2ToyNode::getOutputPort(const std::string& name) const throw(YACS::Exception)
{
  if(name==NAME_SEQ_OUTPRT)
    return (OutputPort *)&_seqOut;
  else
    return ElementaryNode::getOutputPort(name);
}

Node *Seq2ToyNode::simpleClone(ComposedNode *father, bool editionOnly) const
{
  return new Seq2ToyNode(*this,father);
}

void Seq3ToyNode::execute()
{
  SequenceAny *vals1=(SequenceAny *)_inValue1.getValue();
  SequenceAny *vals2=(SequenceAny *)_inValue2.getValue();
  int val=vals1->size();
  TypeCodeSeq *tc1=new TypeCodeSeq("","",Runtime::_tc_int);
  SequenceAny *tmp=SequenceAny::New(tc1,val);
  tc1->decrRef();
  for(int i=0;i<val;i++)
    {
      Any *anyTemp=(Any *)(*vals1)[i];
      SequenceAny *anyTemp2=(SequenceAny *)anyTemp;
      int val2=anyTemp2->size();
      SequenceAny *tmp2=SequenceAny::New(Runtime::_tc_int,val2);
      for(int j=0;j<val2;j++)
        {
          Any *tmp3=AtomAny::New((int)((*vals1)[i][j]->getDoubleValue()+(*vals2)[i][j]->getDoubleValue()));
          tmp2->setEltAtRank(j,tmp3);
          tmp3->decrRef();
        }
      tmp->setEltAtRank(i,tmp2);
      tmp2->decrRef();
    }
  _seqOut.put((const void *)tmp);
  tmp->decrRef();
}

Seq3ToyNode::Seq3ToyNode(const Seq3ToyNode& other, ComposedNode *father):ElementaryNode(other,father),
                                                                        _seqOut(other._seqOut,this),
                                                                        _inValue1(other._inValue1,this),
                                                                        _inValue2(other._inValue1,this)
{
}

Seq3ToyNode::Seq3ToyNode(const std::string& name):ElementaryNode(name),_seqOut(NAME_SEQ_OUTPRT,this,Runtime::_tc_double),
                                                  _inValue1(NAME_SEQ_INPRT1,this,Runtime::_tc_int),
                                                  _inValue2(NAME_SEQ_INPRT2,this,Runtime::_tc_int)
{
  _implementation=ToyNode::MY_IMPL_NAME;
  TypeCodeSeq *tc1=new TypeCodeSeq("","",Runtime::_tc_double);
  TypeCodeSeq *tc2=new TypeCodeSeq("","",tc1);
  tc1->decrRef();
  _inValue1.edSetType(tc2);
  _inValue2.edSetType(tc2);
  tc2->decrRef();
  tc1=new TypeCodeSeq("","",Runtime::_tc_int);
  tc2=new TypeCodeSeq("","",tc1);
  tc1->decrRef();
  _seqOut.edSetType(tc2);
  tc2->decrRef();
}

int Seq3ToyNode::getNumberOfInputPorts() const
{
  return ElementaryNode::getNumberOfOutputPorts()+2;
}

std::list<InputPort *> Seq3ToyNode::getSetOfInputPort() const
{
  list<InputPort *> ret=ElementaryNode::getSetOfInputPort();
  ret.push_back((InputPort *)&_inValue1);
  ret.push_back((InputPort *)&_inValue2);
  return ret;
}

InputPort *Seq3ToyNode::getInputPort(const std::string& name) const throw(YACS::Exception)
{
  if(name==NAME_SEQ_INPRT1)
    return (InputPort *)&_inValue1;
  else if(name==NAME_SEQ_INPRT2)
    return (InputPort *)&_inValue2;
  else
    return ElementaryNode::getInputPort(name);
}

int Seq3ToyNode::getNumberOfOutputPorts() const
{
  return ElementaryNode::getNumberOfOutputPorts()+1;
}

std::list<OutputPort *> Seq3ToyNode::getSetOfOutputPort() const
{
  list<OutputPort *> ret=ElementaryNode::getSetOfOutputPort();
  ret.push_back((OutputPort *)&_seqOut);
  return ret;
}

OutputPort *Seq3ToyNode::getOutputPort(const std::string& name) const throw(YACS::Exception)
{
  if(name==NAME_SEQ_OUTPRT)
    return (OutputPort *)&_seqOut;
  else
    return ElementaryNode::getOutputPort(name);
}

Node *Seq3ToyNode::simpleClone(ComposedNode *father, bool editionOnly) const
{
  return new Seq3ToyNode(*this,father);
}

InputLimitPort::InputLimitPort(const InputLimitPort& other, Node *newHelder):InputPort(other,newHelder),
                                                                             DataPort(other,newHelder),Port(other,newHelder),
                                                                             _data(0),_initData(0)
{
  if(other._data)
    _data=other._data->clone();
  if(other._initData)
    _initData=other._initData->clone();
}

InputLimitPort::InputLimitPort(const string& name, Node *node)
                :InputPort(name, node, Runtime::_tc_double),
                DataPort(name, node, Runtime::_tc_double),
                Port(node),_data(0),_initData(0)
{
}

void InputLimitPort::put(const void *data) throw(ConversionException)
{
  put((Any *)data);
}

InputPort *InputLimitPort::clone(Node *newHelder) const
{
  return new InputLimitPort(*this,newHelder);
}

bool InputLimitPort::edIsManuallyInitialized() const
{
  return _initData!=0;
}

void *InputLimitPort::get() const throw(YACS::Exception)
{
  if(!_data)
    {
      string what="InputLimitPort::get : no value currently in input whith name \""; what+=_name; what+="\"";
      throw Exception(what);
    }
  return (void *)_data;
}

void InputLimitPort::edRemoveManInit()
{
  if(_initData)
    _initData->decrRef();
  _initData=0;
  InputPort::edRemoveManInit();
}

void InputLimitPort::put(Any *data)
{
  if(_data)
    _data->decrRef();
  _data=data;
  _data->incrRef();
}

void InputLimitPort::exSaveInit()
{
  if(_initData) _initData->decrRef();
  _initData=_data;
  _initData->incrRef();
}

void InputLimitPort::exRestoreInit()
{
  if(!_initData)return;
  if(_data) _data->decrRef();
  _data=_initData;
  _data->incrRef();
}

InputLimitPort::~InputLimitPort()
{
  if(_data)
    _data->decrRef();
  if(_initData)
    _initData->decrRef();
}

OutputLimitPort::OutputLimitPort(const OutputLimitPort& other, Node *newHelder):OutputPort(other,newHelder),
                                                                                DataPort(other,newHelder),Port(other,newHelder),_data(0)
{
  if(other._data)
    _data=other._data->clone();
}

OutputLimitPort::OutputLimitPort(const string& name, Node *node, TypeCode *type):OutputPort(name, node, type),DataPort(name, node, type),Port(node),_data(0)
{
}

void OutputLimitPort::put(const void *data) throw(ConversionException)
{
  put((Any *)data);
  OutputPort::put(data);
}

OutputPort *OutputLimitPort::clone(Node *newHelder) const
{
  return new OutputLimitPort(*this,newHelder);
}

void OutputLimitPort::put(Any *data)
{
  if(_data)
    _data->decrRef();
  _data=data;
  _data->incrRef();
}

OutputLimitPort::~OutputLimitPort()
{
  if(_data)
    _data->decrRef();
}

void LimitNode::init(bool start)
{
  if(start)
    _current=0.;
  Node::init(start);
}

void LimitNode::execute()
{
  _current+=_entry.getAny()->getDoubleValue();
  Any *tmp=AtomAny::New(_current);
  _counterPort.put((const void *)tmp);
  tmp->decrRef();
  bool verdict=(_current<_limit);
  tmp=AtomAny::New(verdict);
  _switchPort.put((const void *)tmp);
  tmp->decrRef();
}

Node *LimitNode::simpleClone(ComposedNode *father, bool editionOnly) const
{
  return new LimitNode(*this,father);
}

std::list<InputPort *> LimitNode::getSetOfInputPort() const
{
  list<InputPort *> ret=ElementaryNode::getSetOfInputPort();
  ret.push_back((InputPort *)&_entry);
  return ret;
}

std::list<OutputPort *> LimitNode::getSetOfOutputPort() const
{
  list<OutputPort *> ret=ElementaryNode::getSetOfOutputPort();
  ret.push_back((OutputPort *)&_switchPort);
  ret.push_back((OutputPort *)&_counterPort);
  return ret;
}

InputPort *LimitNode::getInputPort(const std::string& name) const throw(YACS::Exception)
{
  if(name==NAME_FOR_SWPORT)
    return (InputPort *)&_entry;
  else
    return ElementaryNode::getInputPort(name);
}

OutputPort *LimitNode::getOutputPort(const std::string& name) const throw(YACS::Exception)
{
  if(name==NAME_FOR_SWPORT)
    return (OutputPort *)&_switchPort;
  else if(name==NAME_FOR_SWPORT2)
    return (OutputPort *)&_counterPort;
  else
    return ElementaryNode::getOutputPort(name);
}

LimitNode::LimitNode(const LimitNode& other, ComposedNode *father):ElementaryNode(other,father),
                                                                   _limit(other._limit),_current(other._limit),
                                                                   _entry(other._entry,this),
                                                                   _switchPort(other._switchPort,this),
                                                                   _counterPort(other._counterPort,this)
{
}

LimitNode::LimitNode(const string& name):ElementaryNode(name),_limit(0.),_current(0.),
                                         _entry(NAME_FOR_SWPORT,this),
                                         _switchPort(NAME_FOR_SWPORT,this, Runtime::_tc_bool),
                                         _counterPort(NAME_FOR_SWPORT2,this, Runtime::_tc_double)
{
}
