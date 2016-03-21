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

#ifndef __SHAREDPTR_HXX__
#define __SHAREDPTR_HXX__

/*!
 * \brief: Allow to manage memory of instances of T.
 * The only constraint on T is to have method incrRef and DecrRef.
 * Typically T inherits from YACS::ENGINE::RefCounter.
 */
template<class T>
class SharedPtr
{
private:
  T *_ptr;
public:
  SharedPtr(T *ptr):_ptr(ptr) { }
  SharedPtr(const SharedPtr<T>& other):_ptr(other._ptr) { _ptr->incrRef(); }
  SharedPtr<T> &operator=(const SharedPtr<T>& other);
  T &operator*() { return *_ptr; }
  const T &operator*() const { return *_ptr; }
  T *operator->() { return _ptr; }
  const T *operator->() const { return _ptr; }
  ~SharedPtr() { _ptr->decrRef(); }
  operator T *() { return _ptr; }
  operator const T *() const { return _ptr; }
  operator T &() { return *_ptr; }
  operator const T &() const { return *_ptr; }
  SharedPtr<T> operator[](int i) const;
  SharedPtr<T> operator[](const char *key) const;
};

template<class T>
SharedPtr<T> &SharedPtr<T>::operator=(const SharedPtr<T>& other)
{
  _ptr->decrRef();
  _ptr=other._ptr;
  _ptr->incrRef();
  return *this;
}

template<class T>
SharedPtr<T> SharedPtr<T>::operator[](int i) const
{
  return (*_ptr)[i];
}

template<class T>
SharedPtr<T> SharedPtr<T>::operator[](const char *key) const
{
  return (*_ptr)[key];
}

#endif
