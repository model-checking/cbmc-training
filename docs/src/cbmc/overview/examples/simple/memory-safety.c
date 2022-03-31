// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: MIT-0

char buffer[20];

char read_buffer(int i)  { return buffer[i];     }
char read_pointer(int i) { return *(buffer + i); }

main() {
  int index;
  read_buffer(index);
  read_pointer(index);
}
