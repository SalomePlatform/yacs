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
};

template<class T>
SharedPtr<T> &SharedPtr<T>::operator=(const SharedPtr<T>& other)
{
  _ptr->decrRef();
  _ptr=other._ptr;
  _ptr->incrRef();
}

template<class T>
SharedPtr<T> SharedPtr<T>::operator[](int i) const
{
  return (*_ptr)[i];
}

#endif
