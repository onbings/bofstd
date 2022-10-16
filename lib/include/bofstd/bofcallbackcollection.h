/*
 * Copyright (c) 2015-2020, Onbings. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module defines the BofCallbackCollection class
 *
 * Name:        bofcallback.h
 * Author:      Bernard HARMEL: onbings@dscloud.me
 * Web:			    onbings.dscloud.me
 * Revision:    1.0
 *
 * Rem:         Nothing
 *
 * History:
 *
 * V 1.00  Nov 22 2018  BHA : Initial release
 */
#pragma once

#include <bofstd/bofsystem.h>

#include <atomic>
#include <functional>
#include <map>

BEGIN_BOF_NAMESPACE()

//https://www.fluentcpp.com/2018/05/15/make-sfinae-pretty-1-what-value-sfinae-brings-to-code/
//https://stackoverflow.com/questions/6972368/stdenable-if-to-conditionally-compile-a-member-function
//template< typename T>
template<typename T> //,	typename = typename std::enable_if<std::is_function<typename std::remove_pointer<T>::type>::value	>::type>
class BofCallbackCollection
{

private:
  struct CB_ITEM
  {
    //				bool EraseIt_B;
    T Callback;

    CB_ITEM()
    {
      Reset();
    }

    void Reset()
    {
      //					EraseIt_B=false;
      Callback = nullptr;
    }
  };

  std::atomic<uint32_t> mCallbackId;
  std::map<uint32_t, CB_ITEM> mCallbackCollection;
  BOF_MUTEX mMtxCallbackCollection_X;
  bool mUnregisterIfFail_B = false;

  BofCallbackCollection()
  {
    BOFERR Sts_E = Bof_CreateMutex("BofCallbackCollection", true, true, mMtxCallbackCollection_X);
    BOF_ASSERT(Sts_E == BOF_ERR_NO_ERROR);

    mCallbackId.store(0);
  }

public:
  virtual ~BofCallbackCollection()
  {
    Bof_DestroyMutex(mMtxCallbackCollection_X);
  }

  static BofCallbackCollection<T> &S_Instance()
  {
    // Since it's a static variable, if the class has already been created,
    // It won't be created again.
    // And it **is** thread-safe in C++11.
    static BofCallbackCollection<T> mTheInstance;

    return mTheInstance;
  }

  BofCallbackCollection(BofCallbackCollection const &) = delete; // Copy construct
  BofCallbackCollection(BofCallbackCollection &&) = delete; // Move construct
  BofCallbackCollection &operator=(BofCallbackCollection const &) = delete; // Copy assign
  BofCallbackCollection &operator=(BofCallbackCollection &&) = delete; // Move assign
  BOFERR Register(const T &_rCallback, uint32_t &_rId_U32)
  {
    BOFERR Rts_E = Bof_LockMutex(mMtxCallbackCollection_X);
    CB_ITEM Cb_X;

    if (Rts_E == BOF_ERR_NO_ERROR)
    {
      _rId_U32 = ++mCallbackId;
      //			Cb_X.EraseIt_B=false;
      Cb_X.Callback = _rCallback;
      mCallbackCollection[_rId_U32] = Cb_X;
      Bof_UnlockMutex(mMtxCallbackCollection_X);
    }
    return Rts_E;
  }

  BOFERR Unregister(uint32_t _Id_U32)
  {
    BOFERR Rts_E = Bof_LockMutex(mMtxCallbackCollection_X);

    if (Rts_E == BOF_ERR_NO_ERROR)
    {
      Rts_E = BOF_ERR_NOT_FOUND;
      auto It = mCallbackCollection.find(_Id_U32);
      if (It != mCallbackCollection.end())
      {
        mCallbackCollection.erase(It);
        Rts_E = BOF_ERR_NO_ERROR;
        //					It->second.EraseIt_B=true;
      }
      Bof_UnlockMutex(mMtxCallbackCollection_X);
    }
    return Rts_E;
  }

  template<typename ... Args>
  BOFERR Call(uint32_t _Id_U32, const Args &... _Args)
  {
    bool HasFailed_B;

    BOFERR Rts_E = Bof_LockMutex(mMtxCallbackCollection_X);
    if (Rts_E == BOF_ERR_NO_ERROR)
    {
      Rts_E = BOF_ERR_NOT_FOUND;
      auto It = mCallbackCollection.find(_Id_U32);
      if (It != mCallbackCollection.end())
      {
        /*
                  if (It->second.EraseIt_B)
                  {
                    mCallbackCollection.erase(It);
                  }
                  else
        */
        {
          //				static_assert(std::is_same<typename T::result_type, void>::value, "Oops");
          Rts_E = BOF_ERR_NO_ERROR;
          CallIt(It->second.Callback, HasFailed_B, _Args ...);
          if ((mUnregisterIfFail_B) && (HasFailed_B))
          {
            mCallbackCollection.erase(It);
            Rts_E = BOF_ERR_CANCEL;
          }
        }
      }
      Bof_UnlockMutex(mMtxCallbackCollection_X);
    }
    return Rts_E;
  }

  template<typename ... Args>
  BOFERR Call(const Args &... _Args)
  {
    BOFERR Rts_E = Bof_LockMutex(mMtxCallbackCollection_X);
    bool HasFailed_B;

    if (Rts_E == BOF_ERR_NO_ERROR)
    {
      auto It = mCallbackCollection.begin();
      Rts_E = (It != mCallbackCollection.end()) ? BOF_ERR_NO_ERROR : BOF_ERR_EMPTY;

      while (It != mCallbackCollection.end())
      {
        /*
                  if (It->second.EraseIt_B)
                  {
                    It=mCallbackCollection.erase(It);
                  }
                  else
        */
        {
          CallIt(It->second.Callback, HasFailed_B, _Args ...);
          if ((mUnregisterIfFail_B) && (HasFailed_B))
          {
            It = mCallbackCollection.erase(It);
          }
          else
          {
            ++It;
          }
        }
      }
      Bof_UnlockMutex(mMtxCallbackCollection_X);
    }
    return Rts_E;
  }

  template<typename Q = T, typename ... Args>
  typename std::enable_if<std::is_integral<typename Q::result_type>::value || std::is_pointer<typename Q::result_type>::value || std::is_enum<typename Q::result_type>::value, void>::type
    UnregisterIfFail(bool _UnregisterIfFail_B)
  {
    mUnregisterIfFail_B = _UnregisterIfFail_B;
  }

private:
  //public:
  //If we call do_stuff1 T Must be a function returning a void value
  //		void do_stuff1(int, typename std::enable_if<std::is_same<typename T::result_type, void>::value,	std::nullptr_t>::type = nullptr);
  //		{	printf("do_stuff1 void(%d)\n", _Arg_i);	}
  //If we call do_stuff2 T Must be a function returning an int value
  //		int do_stuff2(int, typename std::enable_if<std::is_same<typename T::result_type, int>::value,	std::nullptr_t>::type = nullptr);
  //		{	printf(""do_stuff2 int(%d)\n",_Arg_i); return _Arg_i*2;	}

  template<typename Q = T, typename ... Args>
  typename std::enable_if<std::is_same<typename Q::result_type, void>::value, void>::type
    CallIt(const T &_rFunction, bool &_rHasFailed_B, const Args &... _Args)
  {
    _rHasFailed_B = false;
    _rFunction(_Args ...);
  }

  //typename std::enable_if<!std::is_same<typename Q::result_type, void>::value, typename Q::result_type>::type CallIt(T _Function, const Args &... _Args)
  template<typename Q = T, typename ... Args>
  typename std::enable_if<
    std::is_integral<typename Q::result_type>::value || std::is_pointer<typename Q::result_type>::value || std::is_enum<typename Q::result_type>::value, typename Q::result_type>::type
    CallIt(const T &_rFunction, bool &_rHasFailed_B, const Args &... _Args)
  {
    typename Q::result_type Rts;
    Rts = _rFunction(_Args ...);
    _rHasFailed_B = Rts ? true : false;
    return Rts;
  }

};

END_BOF_NAMESPACE()