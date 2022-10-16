/*
 * Copyright (c) 2015-2020, Onbings. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module defines the bofobserver/bofobservable design pattern
 *
 * Name:        bofobserver.h
 * Author:      Bernard HARMEL: onbings@dscloud.me
 * Web:			    onbings.dscloud.me
 * Revision:    1.0
 *
 * Rem:         http://come-david.developpez.com/tutoriels/dps/?page=BofObservator
 *
 * History:
 *
 * V 1.00  Dec 26 2013  BHA : Initial release
 */
#pragma once

#include "bofstd/bofstd.h"

#include <list>
#include <mutex>

typedef int Info;

class BofObservable;

class BOFSTD_EXPORT BofObserver
{
protected:
  std::mutex mCsObserver_O;
  std::list<BofObservable *> mListOfObservable_O;
  typedef std::list<BofObservable *>::iterator ObservableIterator;
  typedef std::list<BofObservable *>::const_iterator ConstObservableIterator;

  virtual ~BofObserver() = 0;

public:
  void ObserverNotifyAll(uint64_t _User_U64, void *_pUser);

  virtual void V_ObservableNotify(BofObservable *_pBofObservable_O, uint64_t _User_U64, void *_pUser);

  void RegisterObservable(BofObservable *_pBofObservable_O);

  void UnregisterObservable(BofObservable *_pBofObservable_O);
};

class BOFSTD_EXPORT BofObservable
{
private:
  std::mutex mCsObservable_O;
  std::list<BofObserver *> mListOfObserver_O;

  typedef std::list<BofObserver *>::iterator ObserverIterator;
  typedef std::list<BofObserver *>::const_iterator ConstObserverIterator;

public:
  BofObservable();

  virtual ~BofObservable();

  void RegisterObserver(BofObserver *_pBofObserver_O);

  void UnregisterObserver(BofObserver *_pBofObserver_O);

  // virtual Info Status() const = 0;


protected:
  void ObservableNotify(uint64_t _User_U64, void *_pUser);

public:
  virtual void V_ObserverNotifyAll(BofObserver *_pObserver_O, uint64_t _User_U64, void *_pUser);

};

#if 0
// Example
class Barometre :public   BofObservable
{
  int  pression;
public:
  void Change(int valeur);

  int  Status() const;
};

class Thermometre :public BofObservable
{
  int  temperature;
public:

  void Change(int valeur);

  Info Status() const;
};

class MeteoFrance :public BofObserver
{
};
#endif