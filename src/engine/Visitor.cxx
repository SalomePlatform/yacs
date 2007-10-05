
#include "Visitor.hxx"
#include "Node.hxx"
#include "Proc.hxx"

using namespace YACS::ENGINE;
using namespace std;

Visitor::Visitor(ComposedNode *root): _root(root)
{
}

std::map<std::string, std::string> Visitor::getNodeProperties(Node *node)
{
  return node->_propertyMap;
}

std::map<std::string, TypeCode*> Visitor::getTypeCodeMap(Proc *proc)
{
  return proc->typeMap;
}

std::map<std::string, Container*> Visitor::getContainerMap(Proc *proc)
{
  return proc->containerMap;
}

std::list<TypeCodeObjref *> Visitor::getListOfBases(TypeCodeObjref *objref)
{
  return objref->_listOfBases;
}
