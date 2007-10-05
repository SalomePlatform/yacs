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
