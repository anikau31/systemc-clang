
// Filename: common_header.h

//----------------------------------------------------------------------
//  Copyright (c) 2008 by Doulos Ltd.
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//  http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//----------------------------------------------------------------------

// Version 1  09-Sep-2008


#ifndef __COMMON_HEADER_H__
#define __COMMON_HEADER_H__

#define SC_INCLUDE_DYNAMIC_PROCESSES

#include "systemc"
using namespace sc_core;
using namespace sc_dt;
using namespace std;

#include "tlm.h"
#include "tlm_utils/simple_initiator_socket.h"
#include "tlm_utils/simple_target_socket.h"
#include "tlm_utils/passthrough_target_socket.h"
#include "tlm_utils/multi_passthrough_initiator_socket.h"
#include "tlm_utils/multi_passthrough_target_socket.h"
#include "tlm_utils/peq_with_cb_and_phase.h"
#include "tlm_utils/instance_specific_extensions.h"

#include "mm.h"
#include "tlm2_base_protocol_checker.h"

#include <iomanip>
#include <deque>

static ofstream fout("example.log");

// Generate a random delay (with power-law distribution) to aid testing and stress the protocol
int rand_ps()
{
  int n = rand() % 100;
  n = n * n * n;
  return n / 100;
}

DECLARE_EXTENDED_PHASE(internal_ph);


#endif

