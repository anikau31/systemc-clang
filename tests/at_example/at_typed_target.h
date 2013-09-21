
// Filename: at_typed_target.h

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


#ifndef __AT_TYPED_TARGET_H__
#define __AT_TYPED_TARGET_H__

#include "common_header.h"

//struct AT_typeD_target: sc_module
SC_MODULE(AT_typeD_target)
{
  // The typeD target returns BEGIN_RESP on the return path with TLM_UPDATED, and expects
  // to receive an END_RESP on the forward path.
  // But if forbidden by the response exclusion rule from returning BEGIN_RESP,
  // it sends BEGIN_RESP on the backward path instead

  tlm_utils::simple_target_socket<AT_typeD_target, 32> socket;

  SC_CTOR(AT_typeD_target)
  : socket("socket")
  , response_in_progress(false)
  , next_response_pending(0)
  {
    socket.register_nb_transport_fw(this, &AT_typeD_target::nb_transport_fw);

    SC_METHOD(send_rsp_process);
      sensitive << send_rsp_event;
      dont_initialize();
  }

  virtual tlm::tlm_sync_enum nb_transport_fw( tlm::tlm_generic_payload& trans,
                                              tlm::tlm_phase& phase, sc_time& delay )
  {
    if (phase == tlm::BEGIN_REQ)
    {
      trans.acquire();

      tlm::tlm_command cmd = trans.get_command();
      sc_dt::uint64    adr = trans.get_address();
      unsigned char*   ptr = trans.get_data_ptr();
      unsigned int     len = trans.get_data_length();
      unsigned char*   byt = trans.get_byte_enable_ptr();
      unsigned int     wid = trans.get_streaming_width();

      if (byt != 0) {
        trans.set_response_status( tlm::TLM_BYTE_ENABLE_ERROR_RESPONSE );
        return tlm::TLM_COMPLETED;
      }
      if (len > 4 || wid < len) {
        trans.set_response_status( tlm::TLM_BURST_ERROR_RESPONSE );
        return tlm::TLM_COMPLETED;
      }

      if ( cmd == tlm::TLM_READ_COMMAND )
      {
        *reinterpret_cast<int*>(ptr) = -int(adr);
        fout << hex << adr << " Execute READ, data = " << *reinterpret_cast<int*>(ptr)
             << " in " << name() << endl;
      }
      else if ( cmd == tlm::TLM_WRITE_COMMAND )
        fout << hex << adr << " Execute WRITE, data = " << *reinterpret_cast<int*>(ptr)
             << " in " << name() << endl;

      trans.set_response_status( tlm::TLM_OK_RESPONSE );

      delay = delay + sc_time(rand_ps(), SC_PS); // Latency

      // Target must honor BEGIN_RESP/END_RESP exclusion rule
      // i.e. must not send BEGIN_RESP until receiving previous END_RESP or BEGIN_REQ
      if (response_in_progress)
      {
        // Target allows only two transactions in-flight
        if (next_response_pending)
          SC_REPORT_FATAL("TLM-2", "Attempt to have two pending responses in target");

        next_response_pending = &trans;
        next_response_time    = sc_time_stamp() + delay;
        return tlm::TLM_ACCEPTED;
      }
      else
      {
        response_in_progress = true;
        phase = tlm::BEGIN_RESP;
        return tlm::TLM_UPDATED;
      }
    }
    else if (phase == tlm::END_RESP)
    {
      // On receiving END_RESP, the target can release the transaction
      // and allow other pending transactions to proceed

      if (!response_in_progress)
        SC_REPORT_FATAL("TLM-2", "Illegal transaction phase END_RESP received by target");

      // Target itself is now clear to issue the next BEGIN_RESP
      trans.release();
      response_in_progress = false;
      if (next_response_pending)
      {
        send_rsp_event.notify();
      }
      return tlm::TLM_COMPLETED;
    }
  return tlm::TLM_ACCEPTED;
  }

  void send_rsp_process()
  {
    assert( next_response_pending );
    tlm::tlm_generic_payload* trans = next_response_pending;
    next_response_pending = 0;

    tlm::tlm_phase phase = tlm::BEGIN_RESP;
    sc_time delay;
    response_in_progress = true;

    if (next_response_time < sc_time_stamp())
      delay = SC_ZERO_TIME;
    else
      delay = next_response_time - sc_time_stamp();

    tlm::tlm_sync_enum status;
    status = socket->nb_transport_bw( *trans, phase, delay );

    if (status == tlm::TLM_UPDATED)
    {
      if (phase == tlm::END_RESP)
      {
        trans->release();
        response_in_progress = false;
      }
    }
    else if (status == tlm::TLM_COMPLETED)
    {
      trans->release();
      response_in_progress = false;
    }

  }

  bool  response_in_progress;
  sc_event send_rsp_event;

  tlm::tlm_generic_payload*  next_response_pending;
  sc_time                    next_response_time;
};

#endif

