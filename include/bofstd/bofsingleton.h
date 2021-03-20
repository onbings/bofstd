/*
* Copyright (c) 2015-2020, Onbings. All rights reserved.
*
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
* KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
* PURPOSE.
*
* This module defines the bofBofSingleton class
*
* Name:        bofBofSingleton.h
* Author:      Bernard HARMEL: onbings@dscloud.me
* Web:			    onbings.dscloud.me
* Revision:    1.0
*
* Rem:         http://enki-tech.blogspot.be/2012/08/c11-generic-BofSingleton.html
* 			       http://stackoverflow.com/questions/11711920/how-to-implement-multithread-safe-BofSingleton-in-c11-without-using-mutex
*
* History:
*
* V 1.00  Dec 26 2013  BHA : Initial release
*/

#pragma once

#include <bofstd/bofstd.h>
#include <cassert>

BEGIN_BOF_NAMESPACE()

template<class T>
class BofBofSingleton
{
public:
  static T &S_Instance()
  {
    // Since it's a static variable, if the class has already been created,
    // It won't be created again. And it **is** thread-safe in C++11.
    static T S_mTheInstance;

    return S_mTheInstance;
  }

  // delete copy and move constructors and assign operators
  BofBofSingleton(BofBofSingleton const &) = delete;             // Copy construct
  BofBofSingleton(BofBofSingleton &&) = delete;                  // Move construct
  BofBofSingleton &operator=(BofBofSingleton const &) = delete;  // Copy assign
  BofBofSingleton &operator=(BofBofSingleton &&) = delete;      // Move assign

protected:
  BofBofSingleton()
  {
    printf("Can't instanciate a BofSingleton, use S_Instance()\n");
    abort();
  }
  ~BofBofSingleton() { }
};


template<typename T>
class BofSingleton2
{
public:
  static T& S_Instance();

  BofSingleton2(const BofSingleton2 &) = delete;
  BofSingleton2 &operator=(const BofSingleton2) = delete;

protected:
  BofSingleton2() { }
};

template<typename T>
T& BofSingleton2<T>::S_Instance()
{
  // Since it's a static variable, if the class has already been created, It won't be created again. And it **is** thread-safe in C++11.
  static T S_mTheInstance;
  return S_mTheInstance;
}



//https://www.theimpossiblecode.com/blog/c11-generic-singleton-pattern/

template <typename T, typename CONTEXT>
class BofSingleton3
{
public:
  // Very fast inlined operator accessors
  T* operator->() {return mpInstance;}
  const T* operator->() const {return mpInstance;}
  T& operator*() {return *mpInstance;}
  const T& operator*() const {return *mpInstance;}

  // TODO: delete this method - it was
  //       just used for benchmarking in
  //       the post.
  static T *getInstance()
  {
    static bool static_init = []()->bool {
      mpInstance = new T;
      return true;
    }();
    return mpInstance;
  }

protected:
  BofSingleton3()
  {
    static bool static_init = []()->bool {
      mpInstance = new T;
      return true;
    }();
  }

  BofSingleton3(int)
  {
    static bool static_init = []()->bool {
      mpInstance = CONTEXT::init();
      return true;
    }();
  }

private:
  static T *mpInstance;
};

template <typename T, typename CONTEXT>
T *BofSingleton3<T, CONTEXT>::mpInstance;

END_BOF_NAMESPACE()