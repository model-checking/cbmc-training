// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: MIT-0

int receive() {
  // model receiving an unconstrained POSTIVE integer value from the network
  int msg;
  __CPROVER_assume(msg > 0);
  return msg;
}

void send(int msg) {
  // model sending an integer over the network (nothing to do)
  return;
}
