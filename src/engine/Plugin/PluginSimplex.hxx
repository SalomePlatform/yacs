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

#ifndef __PLUGINSIMPLEX_HXX__
#define __PLUGINSIMPLEX_HXX__

#include "OptimizerAlg.hxx"

#include "decode.hxx"
#include "salomevent.hxx"
#include "maestro.hxx"
#include "simplex.hxx"

extern "C"
{
  YACS::ENGINE::OptimizerAlgBase *PluginSimplexFactory(YACS::ENGINE::Pool *pool);
}

namespace YACS
{
    namespace ENGINE
    {
        class PluginSimplex : public OptimizerAlgSync
        {
        private:
            int _idTest;
            TypeCode *_tc;
            TypeCode *_tcOut;
        protected :
            //Superviseur    *super;
            // distribution
            SalomeEventLoop  *dst;
            LinearDecoder  *dec;
            Maestro  *mtr;
            // swarm
            Simplex  *solv;
        
        public:
            PluginSimplex(Pool *pool);
            virtual ~PluginSimplex();
            TypeCode *getTCForIn() const;
            TypeCode *getTCForOut() const;
            void parseFileToInit(const std::string& fileName);
            void start();
            void takeDecision();
            void initialize(const Any *input) throw(Exception);
            void finish();
        };
    }
}

#endif
