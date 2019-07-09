
// Filename: at_example.cpp

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

/*

AT Example

This example contains multiple AT initiators and targets and an AT interconnect

*/


#include "at_typea_initiator.h"
#include "at_typeb_initiator.h"
#include "at_interconnect.h"
#include "at_typea_target.h"
#include "at_typeb_target.h"
#include "at_typec_target.h"
#include "at_typed_target.h"
#include "at_typee_target.h"


#define initiator0_t AT_typeA_initiator
#define initiator1_t AT_typeA_initiator
#define initiator2_t AT_typeB_initiator
#define initiator3_t AT_typeB_initiator

#define target0_t    AT_typeA_target
#define target1_t    AT_typeB_target
#define target2_t    AT_typeC_target
#define target3_t    AT_typeD_target
#define target4_t    AT_typeE_target


SC_MODULE(Top)
{
  initiator0_t *initiator0;
  initiator1_t *initiator1;
  initiator2_t *initiator2;
  initiator3_t *initiator3;

  AT_interconnect *interconnect;

  target0_t    *target0;
  target1_t    *target1;
  target2_t    *target2;
  target3_t    *target3;
  target4_t    *target4;

  tlm_utils::tlm2_base_protocol_checker<> *check_init0;
  tlm_utils::tlm2_base_protocol_checker<> *check_init1;
  tlm_utils::tlm2_base_protocol_checker<> *check_init2;
  tlm_utils::tlm2_base_protocol_checker<> *check_init3;

  tlm_utils::tlm2_base_protocol_checker<> *check_targ0;
  tlm_utils::tlm2_base_protocol_checker<> *check_targ1;
  tlm_utils::tlm2_base_protocol_checker<> *check_targ2;
  tlm_utils::tlm2_base_protocol_checker<> *check_targ3;
  tlm_utils::tlm2_base_protocol_checker<> *check_targ4;


  SC_CTOR(Top)
  {
    initiator0   = new initiator0_t("initiator0");
    initiator1   = new initiator1_t("initiator1");
    initiator2   = new initiator2_t("initiator2");
    initiator3   = new initiator3_t("initiator3");

    interconnect = new AT_interconnect("interconnect");

    target0      = new target0_t("target0");
    target1      = new target1_t("target1");
    target2      = new target2_t("target2");
    target3      = new target3_t("target3");
    target4      = new target4_t("target4");

    check_init0  = new tlm_utils::tlm2_base_protocol_checker<>("check_init0");
    check_init1  = new tlm_utils::tlm2_base_protocol_checker<>("check_init1");
    check_init2  = new tlm_utils::tlm2_base_protocol_checker<>("check_init2");
    check_init3  = new tlm_utils::tlm2_base_protocol_checker<>("check_init3");

    check_targ0  = new tlm_utils::tlm2_base_protocol_checker<>("check_targ0");
    check_targ1  = new tlm_utils::tlm2_base_protocol_checker<>("check_targ1");
    check_targ2  = new tlm_utils::tlm2_base_protocol_checker<>("check_targ2");
    check_targ3  = new tlm_utils::tlm2_base_protocol_checker<>("check_targ3");
    check_targ4  = new tlm_utils::tlm2_base_protocol_checker<>("check_targ4");

    initiator0->socket.bind(check_init0->target_socket);
    check_init0->initiator_socket.bind(interconnect->targ_socket);

    initiator1->socket.bind(check_init1->target_socket);
    check_init1->initiator_socket.bind(interconnect->targ_socket);

    initiator2->socket.bind(check_init2->target_socket);
    check_init2->initiator_socket.bind(interconnect->targ_socket);

    initiator3->socket.bind(check_init3->target_socket);
    check_init3->initiator_socket.bind(interconnect->targ_socket);

    interconnect->init_socket.bind(check_targ0->target_socket);
    check_targ0->initiator_socket.bind(target0->socket);

    interconnect->init_socket.bind(check_targ1->target_socket);
    check_targ1->initiator_socket.bind(target1->socket);

    interconnect->init_socket.bind(check_targ2->target_socket);
    check_targ2->initiator_socket.bind(target2->socket);

    interconnect->init_socket.bind(check_targ3->target_socket);
    check_targ3->initiator_socket.bind(target3->socket);

    interconnect->init_socket.bind(check_targ4->target_socket);
    check_targ4->initiator_socket.bind(target4->socket);
  }
};


int sc_main(int argc, char* argv[])
{
  Top top("top");
  sc_start();
  return 0;
}
