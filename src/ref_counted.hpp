//
// Copyright Copyright 2009-2019, AMT – The Association For Manufacturing Technology (“AMT”)
// All rights reserved.
//
//    Licensed under the Apache License, Version 2.0 (the "License");
//    you may not use this file except in compliance with the License.
//    You may obtain a copy of the License at
//
//       http://www.apache.org/licenses/LICENSE-2.0
//
//    Unless required by applicable law or agreed to in writing, software
//    distributed under the License is distributed on an "AS IS" BASIS,
//    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//    See the License for the specific language governing permissions and
//    limitations under the License.
//

#pragma once

#include <atomic>

namespace mtconnect
{
  template <class T>
  class RefCountedPtr
  {
   public:
    // Constructors
    RefCountedPtr()
    {
      m_object = nullptr;
    }

    RefCountedPtr(const RefCountedPtr &ptr, bool takeRef = false)
    {
      m_object = nullptr;
      setObject(ptr.getObject(), takeRef);
    }

    RefCountedPtr(T &object, bool takeRef = false)
    {
      m_object = nullptr;
      setObject(&object, takeRef);
    }

    RefCountedPtr(T *object, bool takeRef = false)
    {
      m_object = nullptr;
      setObject(object, takeRef);
    }

    // Destructor
    ~RefCountedPtr();

    // Getters
    T *getObject() const
    {
      return m_object;
    }
    T *operator->() const
    {
      return m_object;
    }
    operator T *() const
    {
      return m_object;
    }

    // Setters
    T *setObject(T *object, bool takeRef = false);
    T *operator=(T *object)
    {
      return setObject(object);
    }
    T *operator=(RefCountedPtr<T> &ptr)
    {
      return setObject(ptr.getObject());
    }

    bool operator==(const RefCountedPtr &another)
    {
      return *m_object == *(another.m_object);
    }

    bool operator<(const RefCountedPtr &another);

   protected:
    T *m_object;
  };

  template <class T>
  inline RefCountedPtr<T>::~RefCountedPtr()
  {
    if (m_object)
      m_object->unrefer();
  }

  template <class T>
  inline bool RefCountedPtr<T>::operator<(const RefCountedPtr<T> &another)
  {
    return (*m_object) < (*another.m_object);
  }

  template <class T>
  inline T *RefCountedPtr<T>::setObject(T *object, bool takeRef)
  {
    if (m_object)
      m_object->unrefer();

    m_object = object;

    if (object && !takeRef)
      m_object->referTo();

    return object;
  }

  class RefCounted
  {
   public:
    RefCounted()
    {
      m_refCount.store(1);
    }

    RefCounted(const RefCounted &aRef)
    {
      m_refCount.store(1);
    }

    virtual ~RefCounted() = default;

    // Reference count management
    void referTo()
    {
      std::atomic_fetch_add(&m_refCount, 1);
    }

    void unrefer()
    {
      std::atomic_fetch_sub(&m_refCount, 1);
      if (m_refCount <= 0)
        delete this;
    }

    unsigned int refCount()
    {
      return m_refCount.load();
    }

   protected:
    // Reference count
    std::atomic_int m_refCount;
  };
}  // namespace mtconnect
