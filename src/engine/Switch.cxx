#include "Switch.hxx"

using namespace YACS::ENGINE;
using namespace std;

Switch::Switch(const string& name):ComposedNode(name)
{
}

Switch::~Switch()
{
}

void Switch::edSetNumberOfCases(int numberOfCases)
{
  _vectorOfNode.resize(numberOfCases);
}

void Switch::edSetNode(int caseId, Node *node) throw(Exception)
{
  if(caseId>=_vectorOfNode.size())
    throw Exception("Switch::edSetNode : caseId is too large compared to number of cases");
  _vectorOfNode[caseId]=node;
}

void Switch::checkNoCyclePassingThrough(Node *node) throw(Exception)
{
  throw Exception("Switch::checkNoCyclePassingThrough : uncorrect control flow link relative to switch");
}
