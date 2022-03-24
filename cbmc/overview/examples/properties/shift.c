// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: MIT-0

main() {
  int x = 1;
  int y = -1;
  int z;
  z = x << 64;
  z = x << -1;
  z = y << 64;
  z = y << -1;
}
