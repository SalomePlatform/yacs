#include "SalomeProc.hxx"
#include "Runtime.hxx"
#include "TypeCode.hxx"
#include <iostream>

using namespace YACS::ENGINE;

TypeCode * SalomeProc::createInterfaceTc(const std::string& id, const std::string& name,
                                   std::list<TypeCodeObjref *> ltc)
{
    std::string myName;
    if(id == "") myName = "IDL:" + name + ":1.0";
    else myName = id;
    return TypeCode::interfaceTc(myName.c_str(),name.c_str(),ltc);
}
TypeCode * SalomeProc::createStructTc(const std::string& id, const std::string& name)
{
  std::string myName;
  if(id == "") myName = "IDL:" + name + ":1.0";
  else myName = id;
  return TypeCode::structTc(myName.c_str(),name.c_str());
}

SalomeProc::~SalomeProc()
{
}

