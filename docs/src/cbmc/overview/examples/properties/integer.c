// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: MIT-0

#include <limits.h>
main() {
  int x;
  int y;
  if (x + y <= INT_MAX)
    x = x + y;
  if (y <= INT_MAX - x)
    x = x + y;
}
