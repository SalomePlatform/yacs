// Copyright (C) 2012-2024  CEA, EDF
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
// Author : Anthony Geay (EDF R&D)

#ifndef __YACSEVALYFXAUTOPTR_HXX__
#define __YACSEVALYFXAUTOPTR_HXX__

namespace YACS
{
  template<class T>
  class AutoPtr
  {
  public:
    AutoPtr(T *ptr=0):_ptr(ptr) {  }
    ~AutoPtr() { destroyPtr(); }
    AutoPtr &operator=(T *ptr) { if(_ptr!=ptr) { destroyPtr(); _ptr=ptr; } return *this; }
    T *operator->() { return _ptr ; }
    const T *operator->() const { return _ptr; }
    T& operator*() { return *_ptr; }
    const T& operator*() const { return *_ptr; }
    operator T *() { return _ptr; }
    operator const T *() const { return _ptr; }
  private:
    void destroyPtr() { delete [] _ptr; }
  private:
    T *_ptr;
  };

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
    T *dettach() { T *ret(_ptr); _ptr=0; return ret; }
  private:
    void destroyPtr() { delete _ptr; }
  private:
    T *_ptr;
  };

  template<class T>
  class AutoCPtr
  {
  public:
    AutoCPtr(T *ptr=0):_ptr(ptr) {  }
    ~AutoCPtr() { destroyPtr(); }
    AutoCPtr &operator=(T *ptr) { if(_ptr!=ptr) { destroyPtr(); _ptr=ptr; } return *this; }
    T *operator->() { return _ptr ; }
    const T *operator->() const { return _ptr; }
    T& operator*() { return *_ptr; }
    const T& operator*() const { return *_ptr; }
    operator T *() { return _ptr; }
    operator const T *() const { return _ptr; }
  private:
    void destroyPtr() { free(_ptr); }
  private:
    T *_ptr;
  };
}

#endif
