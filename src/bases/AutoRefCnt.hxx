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

#ifndef __AUTOREFCNT_HXX__
#define __AUTOREFCNT_HXX__

#include "Exception.hxx"

namespace YACS
{
  namespace BASES
  {
    template<class T>
    class AutoRefCnt
    {
    public:
      AutoRefCnt(const AutoRefCnt& other):_ptr(0) { referPtr(other._ptr); }
      AutoRefCnt(T *ptr=0):_ptr(ptr) { }
      ~AutoRefCnt() { destroyPtr(); }
      bool operator==(const AutoRefCnt& other) const { return _ptr==other._ptr; }
      bool operator==(const T *other) const { return _ptr==other; }
      AutoRefCnt &operator=(const AutoRefCnt& other) { if(_ptr!=other._ptr) { destroyPtr(); referPtr(other._ptr); } return *this; }
      AutoRefCnt &operator=(T *ptr) { if(_ptr!=ptr) { destroyPtr(); _ptr=ptr; } return *this; }
      T *operator->() { return _ptr ; }
      const T *operator->() const { return _ptr; }
      T& operator*() { return *_ptr; }
      const T& operator*() const { return *_ptr; }
      operator T *() { return _ptr; }
      operator const T *() const { return _ptr; }
      T *retn() { if(_ptr) _ptr->incrRef(); return _ptr; }
    private:
      void referPtr(T *ptr) { _ptr=ptr; if(_ptr) _ptr->incrRef(); }
      void destroyPtr() { if(_ptr) _ptr->decrRef(); }
    private:
      T *_ptr;
    };

    template<class T, class U>
    typename YACS::BASES::AutoRefCnt<U> DynamicCast(typename YACS::BASES::AutoRefCnt<T>& autoSubPtr) throw()
    {
      T *subPtr(autoSubPtr);
      U *ptr(dynamic_cast<U *>(subPtr));
      typename YACS::BASES::AutoRefCnt<U> ret(ptr);
      if(ptr)
        ptr->incrRef();
      return ret;
    }

    template<class T, class U>
    typename YACS::BASES::AutoRefCnt<U> DynamicCastSafe(typename YACS::BASES::AutoRefCnt<T>& autoSubPtr)
    {
      T *subPtr(autoSubPtr);
      U *ptr(dynamic_cast<U *>(subPtr));
      if(subPtr && !ptr)
        throw Exception("DynamicCastSafe : U is not a subtype of T !");
      typename YACS::BASES::AutoRefCnt<U> ret(ptr);
      if(ptr)
        ptr->incrRef();
      return ret;
    }

    template<class T>
    class AutoCppPtr
    {
    public:
      AutoCppPtr(T *ptr=0):_ptr(ptr) {  }
      ~AutoCppPtr() { destroyPtr(); }
      AutoCppPtr &operator=(T *ptr) { if(_ptr!=ptr) { destroyPtr(); _ptr=ptr; } return *this; }
      T *operator->() { return _ptr ; }
      const T *operator->() const { return _ptr; }
      T& operator*() { return *_ptr; }
      const T& operator*() const { return *_ptr; }
      operator T *() { return _ptr; }
      operator const T *() const { return _ptr; }
    private:
      void destroyPtr() { delete _ptr; }
    private:
      T *_ptr;
    };
  }
}

#endif
