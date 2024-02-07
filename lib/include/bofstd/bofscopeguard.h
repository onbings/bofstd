#pragma once
#include <bofstd/bofstd.h>
#include <utility>
/*
The code you provided is implementing a simple scope guard utility named BofScopeGuard. A scope guard is a programming idiom used to ensure that a specific operation or set of operations is executed when exiting a scope, regardless of how the scope is exited (through normal execution, return, or an exception).

Let's break down the key components of the BofScopeGuard utility:

BofScopeGuardOnExit Enum Class:

It serves as a tag to indicate that the associated scope guard should be executed on exit.
BofScopeGuard Class Template:

It's a template class that takes a callable object (F) as its template parameter.
The BofScopeGuard object is constructed with a callable object (a lambda function, function pointer, functor, etc.).
The Dismiss method allows you to mark the scope guard as inactive, preventing its execution on exit.
The destructor of BofScopeGuard automatically executes the callable object when the scope is exited, unless dismissed.
operator+ Function:

It's a free function allowing for a convenient way to create BofScopeGuard objects using the + operator.
When BofScopeGuardOnExit is used with +, it creates a BofScopeGuard with the associated callable object.
BOF_SCOPE_EXIT Macro:

This macro provides a convenient way to declare a BofScopeGuard variable within a scope.
It uses a combination of the BOF_ANONYMOUS_VARIABLE macro and the BOF::BofScopeGuardOnExit tag to create an anonymous BofScopeGuard variable.
Here's an example of how you might use this BofScopeGuard utility:

cpp
Copy code
#include "your_scope_guard_header.h"

void exampleFunction()
{
    BOF_SCOPE_EXIT
    {
        // This code will be executed when the scope is exited
        // It can be used for cleanup, logging, or other operations
        std::cout << "Exiting the scope!" << std::endl;
    };

    // Rest of the function...

    // The BofScopeGuard's destructor will be called when exiting the scope
    // The associated lambda function inside BOF_SCOPE_EXIT will be executed
}
In this example, the lambda function provided to BOF_SCOPE_EXIT will be executed when the scope is exited, ensuring that the specified actions are taken regardless of how the scope is exited. This is particularly useful for resource management and cleanup tasks.
*/
BEGIN_BOF_NAMESPACE()
#define BOF_CONCATENATE_IMPL(s1, s2) s1##s2
#define BOF_CONCATENATE(s1, s2) BOF_CONCATENATE_IMPL(s1, s2)
#ifdef __COUNTER__
#define BOF_ANONYMOUS_VARIABLE(str) BOF_CONCATENATE(str, __COUNTER__)
#else
#define BOF_ANONYMOUS_VARIABLE(str) BOF_CONCATENATE(str, __LINE__)
#endif

enum class BofScopeGuardOnExit
{
};
template <typename OutOfScopeCallback>
class BofScopeGuard
{
  OutOfScopeCallback mOutOfScopeCallback;
  bool mActive_B;

public:
  BofScopeGuard() = delete;
  BofScopeGuard(const BofScopeGuard &) = delete;
  BofScopeGuard &operator=(const BofScopeGuard &) = delete;
  BofScopeGuard(BofScopeGuard &&_rrhs) : mF(std::move(_rrhs.mOutOfScopeCallback)), mActive_B(_rrhs.mActive_B)
  {
    _rrhs.dismiss();
  }
  BofScopeGuard(OutOfScopeCallback _OutOfScopeCallback) : mOutOfScopeCallback(std::move(_OutOfScopeCallback)), mActive_B(true)
  {
  }
  ~BofScopeGuard()
  {
    if (mActive_B)
    {
      mOutOfScopeCallback();
    }
  }
  void Dismiss()
  {
    mActive_B = false;
  }
};
template <typename OutOfScopeCallback>
inline BofScopeGuard<OutOfScopeCallback> operator+(BofScopeGuardOnExit, OutOfScopeCallback &&_rrOutOfScopeCallback)
{
  return BofScopeGuard<OutOfScopeCallback>(std::forward<OutOfScopeCallback>(_rrOutOfScopeCallback));
}

#define BOF_SCOPE_EXIT() auto BOF_ANONYMOUS_VARIABLE(BOF_SCOPE_EXIT_STATE) = BOF::BofScopeGuardOnExit() + [&]()
END_BOF_NAMESPACE()