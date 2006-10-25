
#include "CppNode.hxx"
#include "InputPort.hxx"
#include "OutputPort.hxx"

#include <iostream>
#include <set>

using namespace YACS::ENGINE;
using namespace std;

CppNode::CppNode(const string name): ElementaryNode(name)
{
  _implementation = "Cpp";
  cerr << "CppNode::CppNode: " << name << endl;
}

void CppNode::execute()
{
//   // --- Input Ports ready, read data

//   int nbin = _setOfInputPort.size();
//   int nbout = _setOfOutputPort.size();
//   void **input = new (void *)[nbin];
//   void **output = new (void *)[nbout];

//   int i=0;
//   for (std::set<InputPort *>::iterator it = _setOfInputPort.begin(); it != _setOfInputPort.end(); it++)
//     {
//       Data container = (*it)->exGet();
//       void *content = container.get();
//       input[i++] = content;
//     }

//   // --- invoke service (in a thread)

//   bool ret = (_run)(nbin, nbout, input, output);

//   // --- get results, fill OutputPort with copy, then put converted data in linked InputPort

//   i=0;
//   for (std::set<OutputPort *>::iterator it = _setOfOutputPort.begin(); it != _setOfOutputPort.end(); it++)
//     {
//       Data container((*it)->edGetType());
//       container.put(output[i++]);
//       (*it)->put(container);
//     }
}
