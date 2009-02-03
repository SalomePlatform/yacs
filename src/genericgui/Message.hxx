#ifndef _MESSAGE_HXX_
#define _MESSAGE_HXX_

#include <string>

namespace YACS
{
  namespace HMI
  {
    class Message
    {
    public:
      Message(std::string message = "");
      virtual ~Message();
    };
  }
}

#endif
