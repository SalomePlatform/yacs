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

#ifndef __PUGINOPTEVTEST1_HXX__
#define __PUGINOPTEVTEST1_HXX__

#include "OptimizerAlg.hxx"

extern "C"
{
  YACS::ENGINE::OptimizerAlgBase *PluginOptEvTest1Factory(YACS::ENGINE::Pool *pool);
}

namespace YACS
{
  namespace ENGINE
  {
    class PluginOptEvTest1 : public OptimizerAlgSync
    {
    private:
      int _idTest;
      TypeCode *_tcIn;
      TypeCode *_tcOut;
    public:
      PluginOptEvTest1(Pool *pool);
      virtual ~PluginOptEvTest1();
      TypeCode *getTCForIn() const;
      TypeCode *getTCForOut() const;
      TypeCode *getTCForAlgoResult() const;
      Any * getAlgoResult();
      void parseFileToInit(const std::string& fileName);
      void start();
      void takeDecision();
      void initialize(const Any *input) throw(Exception);
      void finish();
    };
  }
}

#endif
