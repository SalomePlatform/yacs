#ifndef _SALOMEPROC_HXX_
#define _SALOMEPROC_HXX_

#include "Proc.hxx"
#include <string>
#include <iostream>
#include <list>

namespace YACS
{
  namespace ENGINE
  {
    class TypeCode;

    class SalomeProc: public Proc
    {
    public:
       SalomeProc(const std::string& name):Proc(name){};
       virtual ~SalomeProc();
       virtual TypeCode * createInterfaceTc(const std::string& id, const std::string& name, 
                                    std::list<TypeCodeObjref *> ltc);
       virtual TypeCode * createStructTc(const std::string& id, const std::string& name);
    };
  }
}

#endif
