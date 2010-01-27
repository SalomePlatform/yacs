//  Copyright (C) 2006-2008  CEA/DEN, EDF R&D
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
#ifndef _SALOMEOPTIMIZERLOOP_HXX_
#define _SALOMEOPTIMIZERLOOP_HXX_

#include "YACSRuntimeSALOMEExport.hxx"
#include "OptimizerLoop.hxx"

namespace YACS
{
  namespace ENGINE
  {
    class YACSRUNTIMESALOME_EXPORT SalomeOptimizerAlgStandardized : public OptimizerAlgStandardized
    {
    public:
      SalomeOptimizerAlgStandardized(Pool *pool, OptimizerAlgBase *alg);
      virtual ~SalomeOptimizerAlgStandardized();
      virtual void parseFileToInit(const std::string& fileName);
      virtual void start();
      virtual void takeDecision();
      virtual TypeCode *getTCForIn() const;
      virtual TypeCode *getTCForOut() const;
      virtual void initialize(const Any *input) throw (Exception);
      virtual void finish();
    private:
      static void *threadFctForAsync(void* ownStack);
    };

    class YACSRUNTIMESALOME_EXPORT SalomeOptimizerLoop: public OptimizerLoop
    {
      protected:
        Node *simpleClone(ComposedNode *father, bool editionOnly) const;
      public:
        SalomeOptimizerLoop(const std::string& name, const std::string& algLibWthOutExt,
                            const std::string& symbolNameToOptimizerAlgBaseInstanceFactory,
                            bool algInitOnFile,bool initAlgo=true);
        SalomeOptimizerLoop(const SalomeOptimizerLoop& other, ComposedNode *father, bool editionOnly);
        ~SalomeOptimizerLoop();
        virtual void setAlgorithm(const std::string& alglib,const std::string& symbol,bool checkLinks=true);
        virtual std::string getAlgLib() const ;
        virtual std::string typeName() {return "YACS__ENGINE__SalomeOptimizerLoop";}
      protected:
        SalomeOptimizerAlgStandardized *_pyalg;
        OptimizerAlgStandardized *_cppalg;
        std::string _alglib;
    };
  }
}

#endif
