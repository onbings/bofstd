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
 * Rem:         http://come-david.developpez.com/tutoriels/dps/?page=BofObserver
 *
 * History:
 *
 * V 1.00  Dec 26 2013  BHA : Initial release
 */
#include "bofstd/bofobserver.h"

#include <algorithm>
#include <iostream>
#include <iterator>
#include <list>

BofObserver::~BofObserver()
{
  // For each observed object, we ask him to remove its current observer
  ConstObservableIterator ItEnd_O = mListOfObservable_O.end();

  for (ObservableIterator It_O = mListOfObservable_O.begin(); It_O != ItEnd_O; ++It_O)
  {
    (*It_O)->UnregisterObserver(this);
  }
}

void BofObserver::V_ObservableNotify(BofObservable * /*_pObservable_O*/, uint64_t /*_User_U64*/, void * /*_pUser*/)
{
}

void BofObserver::RegisterObservable(BofObservable *_pBofObservable_O)
{
  std::lock_guard<std::mutex> Lock_O(mCsObserver_O);
  mListOfObservable_O.push_back(_pBofObservable_O);
}

void BofObserver::UnregisterObservable(BofObservable *_pBofObservable_O)
{
  // Remove observed object.
  if (mListOfObservable_O.size())
  {
    ObservableIterator It_O = std::find(mListOfObservable_O.begin(), mListOfObservable_O.end(), _pBofObservable_O);

    if (It_O != mListOfObservable_O.end())
    {
      std::lock_guard<std::mutex> Lock_O(mCsObserver_O);
      mListOfObservable_O.erase(It_O);
    }
  }
}

void BofObserver::ObserverNotifyAll(uint64_t _User_U64, void *_pUser)
{
  // Notify each observer that the value has changed
  ObservableIterator It_O = mListOfObservable_O.begin();
  ConstObservableIterator ItEnd_O = mListOfObservable_O.end();

  for (; It_O != ItEnd_O; ++It_O)
  {
    (*It_O)->V_ObserverNotifyAll(this, _User_U64, _pUser);
  }
}

BofObservable::BofObservable()
{
}

BofObservable::~BofObservable()
{
  // Idem BofObserver::~BofObserver
  ObserverIterator It_O = mListOfObserver_O.begin();
  ConstObserverIterator ItEnd_O = mListOfObserver_O.end();

  for (; It_O != ItEnd_O; ++It_O)
  {
    (*It_O)->UnregisterObservable(this);
  }
}

void BofObservable::RegisterObserver(BofObserver *_pBofObserver_O)
{
  // Add observer to list
  {

    std::lock_guard<std::mutex> Lock_O(mCsObservable_O);
    mListOfObserver_O.push_back(_pBofObserver_O);
  }
  // And we give him a new observed object.
  _pBofObserver_O->RegisterObservable(this);
}

void BofObservable::UnregisterObserver(BofObserver *_pBofObserver_O)
{
  // Idem BofObserver::RegisterObserver

  if (mListOfObserver_O.size())
  {
    ObserverIterator It_O = std::find(mListOfObserver_O.begin(), mListOfObserver_O.end(), _pBofObserver_O);

    if (It_O != mListOfObserver_O.end())
    {
      std::lock_guard<std::mutex> Lock_O(mCsObservable_O);
      mListOfObserver_O.erase(It_O);
    }
  }
}

void BofObservable::V_ObserverNotifyAll(BofObserver * /*_pObserver_O*/, uint64_t /*_User_U64*/, void * /*_pUser*/)
{
}

void BofObservable::ObservableNotify(uint64_t _User_U64, void *_pUser)
{
  // Notify each observer that the value has changed
  ObserverIterator It_O = mListOfObserver_O.begin();
  ConstObserverIterator ItEnd_O = mListOfObserver_O.end();

  for (; It_O != ItEnd_O; ++It_O)
  {
    (*It_O)->V_ObservableNotify(this, _User_U64, _pUser);
  }
}

#if 0
// Example
void Barometre::Change(int _Valeur_i)
{
  pression = _Valeur_i;
  Notify();
}


int Barometre::Status() const
{
  return pression;
}


void Thermometre::Change(int _Valeur_i)
{
  temperature = _Valeur_i;
  Notify();
}


Info Thermometre::Status() const
{
  return temperature;
}
#endif

#if 0
// App
Barometre   barometre;
Thermometre thermometre;
// un faux bloc pour limiter la port�e de la station
{
  MeteoFrance station;

  thermometre.RegisterObserver(&station);
  barometre.RegisterObserver(&station);

  thermometre.Change(31);
  barometre.Change(975);
}

thermometre.Change(45);
#endif