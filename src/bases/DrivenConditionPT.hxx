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
#ifndef __DRIVENCONDITIONPT_HXX__
#define __DRIVENCONDITIONPT_HXX__

#include "YACSBasesExport.hxx"

#include <pthread.h>

namespace YACS
{
  namespace BASES
  {
    class YACSBASES_EXPORT DrivenConditionPT
    {
    public:
      DrivenConditionPT();
      ~DrivenConditionPT();
      //On master thread
      void waitForAWait();
      void notifyOneSync();
      //On slave thread
      void wait(); //wait on condition
      void signal(); //release the condition
    private:
      pthread_cond_t _cond1;
      pthread_cond_t _cond2;
      pthread_mutex_t _mutexDesc1;
      pthread_mutex_t _mutexDesc2;
    };
  }
}

#endif
