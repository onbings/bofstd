/*
 * Copyright (c) 2023-2033, OnBings All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements the unit testing of the bofscopeguard functions
 *
 * Name:        ut_graph.cpp
 * Author:      Bernard HARMEL: onbings@dscloud.me
 * Web:					onbings.dscloud.me
 * Revision:    1.0
 *
 * Rem:         Initial version from https://github.com/Nelarius/imnodes
 *
 * History:
 *
 * V 1.00  Feb 5 2024  BHA : Initial release
 */
#include "gtestrunner.h"
#include <bofstd/bofscopeguard.h>

static int S_Counter_i = 0;
int CleanUp()
{
  S_Counter_i++;
  return 0;
}
void ScopedGuard()
{
  BOF::BofScopeGuard ScopeGuard(CleanUp);
  BOF_SCOPE_EXIT()
  {
    // This code will be executed when the scope is exited
    // It can be used for cleanup, logging, or other operations
    printf("Exiting the scope !\n");
  };

  // Rest of the function...

  // The BofScopeGuard's destructor will be called when exiting the scope
  // The associated lambda function inside BOF_SCOPE_EXIT will be executed

  auto CleanupLambda = [&]()
  {
    S_Counter_i++;
    printf("Performing cleanup on exit.\n");;
  };

  auto BOF_ANONYMOUS_VARIABLE(BOF_SCOPE_EXIT_STATE) = BOF::BofScopeGuardOnExit() + CleanupLambda;

  // The BofScopeGuard's destructor will be called when exiting the scope
  // The associated lambda function (cleanupLambda) inside the BofScopeGuard will be executed

}
TEST(ScopedGuard_Test, ScopedGuard)
{
  S_Counter_i = 0;
  ScopedGuard();
  ASSERT_EQ(S_Counter_i, 2);
}