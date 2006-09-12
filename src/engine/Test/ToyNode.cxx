#include "ToyNode.hxx"
#include "InputPort.hxx"
#include "OutputPort.hxx"

#include <iostream>

using namespace std;

#define TAB_LENGTH 100

using namespace YACS::ENGINE;

ToyNode::ToyNode(const std::string& name, int time):ElementaryNode(name),_time(time)
{
}
InputPort *ToyNode::edAddInputPort(const std::string& inputPortName)
{
  return edAddInputPort(inputPortName, YACS::Double);
}

OutputPort *ToyNode::edAddOutputPort(const std::string& outputPortName)
{
  return edAddOutputPort(outputPortName, YACS::Double);
}

InputPort *ToyNode::edAddInputPort(const std::string& inputPortName, YACS::DynType type) throw(Exception)
{
  if(type!=YACS::Double)
    throw Exception("ToyNode only deals double dataflow ports");
  return ElementaryNode::edAddInputPort(inputPortName,type);
}

OutputPort *ToyNode::edAddOutputPort(const std::string& outputPortName, YACS::DynType type) throw(Exception)
{
  if(type!=YACS::Double)
    throw Exception("ToyNode only deals double dataflow ports");
  return ElementaryNode::edAddOutputPort(outputPortName,type);
}

InputDataStreamPort *ToyNode::edAddInputDataStreamPort(const std::string& inputPortDSName, YACS::StreamType type) throw(Exception)
{
  throw Exception("ToyNode Not designed to support DataStream");
}

OutputDataStreamPort *ToyNode::edAddOutputDataStreamPort(const std::string& outputPortDSName, YACS::StreamType type) throw(Exception)
{
  throw Exception("ToyNode Not designed to support DataStream");
}

void ToyNode::execute()
{
  std::list<InputPort *>::iterator iter;
  double sum=0.;
  for(iter=_listOfInputPort.begin();iter!=_listOfInputPort.end();iter++)
    sum+=(*iter)->exGet().exGet<double>();
  cerr << "Node " << _name << " executing " << sum << endl;
  double tab[TAB_LENGTH];
  if(_time!=0)
    {
      for(int i=0;i<100000;i++)
	for(int j=0;j<20*_time;j++)
	  tab[((i+j)/2)%TAB_LENGTH]=i*i*j;
    }
  cerr << "Node " << _name << " executing " << sum << endl;
  if(_listOfOutputPort.empty())
    return;
  sum/=_listOfOutputPort.size();
  std::list<OutputPort *>::iterator iter2;
  for(iter2=_listOfOutputPort.begin();iter2!=_listOfOutputPort.end();iter2++)
    (*iter2)->exPut(sum);
}

