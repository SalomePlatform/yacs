// Copyright (C) 2006-2012  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
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

#ifndef __YACS_CppCOMPONENT__
#define __YACS_CppCOMPONENT__

#include <string>
#include "ComponentInstance.hxx"

namespace YACS 
{
  namespace ENGINE
  {

    class Any;

    struct returnInfo {
         int code;
         std::string message;
    };

    typedef void * (*InitFunction)();
    typedef void (*RunFunction) (void *, const char *, int, int, Any **, Any **, returnInfo *);
    typedef void (*TerminateFunction)(void **);
    typedef void (*PingFunction) ();
        
    class CppComponent : public ComponentInstance {
      public:

        CppComponent(const std::string & name);
        CppComponent(void * obj, RunFunction r, TerminateFunction t, 
                     const std::string & name) 
             : __obj(obj), __run(r), __terminate(t), ComponentInstance(name) {}
        CppComponent(const CppComponent& other);
        virtual ~CppComponent();

        void run(const char * service, int nbIn, int nbOut,
                         Any ** argIn, Any ** argOut) throw (YACS::Exception);

        static const char KIND[];
        virtual std::string getKind() const;
        virtual void load();
        virtual void unload();
        virtual bool isLoaded();
        virtual ServiceNode* createNode(const std::string& name);
        virtual YACS::ENGINE::ComponentInstance* clone() const;
        
      protected:
  
        void * __obj;
        YACS::ENGINE::RunFunction __run;
        YACS::ENGINE::TerminateFunction __terminate;
    };
  };
};

#endif
