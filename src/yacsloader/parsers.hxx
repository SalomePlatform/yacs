#ifndef _PARSERS_HXX_
#define _PARSERS_HXX_

#include <map>
#include <parserBase.hxx>

namespace YACS
{
  namespace ENGINE
  {
    class Proc;
  }

  class YACSLoader
  {
    public:
        YACSLoader();
        virtual ~YACSLoader();
        virtual YACS::ENGINE::Proc* load(const char * filename);
        void registerProcCataLoader();

    protected:
        std::map<std::string,parser*> _defaultParsersMap;
  };
}

#endif
