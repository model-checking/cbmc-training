// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: MIT-0

int quartile(int x) {
  int y;

  if (x < 0 || x > 99) return 0;

  if (x < 50)
    if (x < 25)
      y = 1;
    else
      y = 2;
  else
    if (x < 75)
      y = 3;
    else
      y = 4;

  return y;
}
