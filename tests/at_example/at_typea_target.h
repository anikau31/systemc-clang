
// Filename: at_typea_target.h

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


#ifndef __AT_TYPEA_TARGET_H__
#define __AT_TYPEA_TARGET_H__

#include "common_header.h"

//struct AT_typeA_target: sc_module
SC_MODULE(AT_typeA_target)
{
  // The typeA target returns both END_REQ and BEGIN_RESP on the backward path
  // It does not use the return path at all.

  tlm_utils::simple_target_socket<AT_typeA_target, 32> socket;

  SC_CTOR(AT_typeA_target)
  : socket("socket")
  , n_trans(0)
  , response_in_progress(false)
  , next_response_pending(0)
  , end_req_pending(0)
  , m_peq(this, &AT_typeA_target::peq_cb)
  {
    socket.register_nb_transport_fw(this, &AT_typeA_target::nb_transport_fw);
  }

  virtual tlm::tlm_sync_enum nb_transport_fw( tlm::tlm_generic_payload& trans,
                                              tlm::tlm_phase& phase, sc_time& delay )
  {
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

    m_peq.notify( trans, phase, delay);
    return tlm::TLM_ACCEPTED;
  }

  void peq_cb(tlm::tlm_generic_payload& trans, const tlm::tlm_phase& phase)
  {
    sc_time delay;

    switch (phase) {
    case tlm::BEGIN_REQ:

      trans.acquire();

      // Put back-pressure on initiator by deferring END_REQ until pipeline is clear
      if (n_trans == 2)
        end_req_pending = &trans;
      else
      {
        send_end_req(trans);
      }

      break;

    case tlm::END_RESP:
      // On receiving END_RESP, the target can release the transaction
      // and allow other pending transactions to proceed

      if (!response_in_progress)
        SC_REPORT_FATAL("TLM-2", "Illegal transaction phase END_RESP received by target");

      trans.release();
      n_trans--;

      // Target itself is now clear to issue the next BEGIN_RESP
      response_in_progress = false;
      if (next_response_pending)
      {
        send_response( *next_response_pending );
        next_response_pending = 0;
      }

      // ... and to unblock the initiator by issuing END_REQ
      if (end_req_pending)
      {
        send_end_req( *end_req_pending );
        end_req_pending = 0;
      }

      break;

    case tlm::END_REQ:
    case tlm::BEGIN_RESP:
      SC_REPORT_FATAL("TLM-2", "Illegal transaction phase received by target");
      break;

    default:
      if (phase == internal_ph)
      {
        tlm::tlm_command cmd = trans.get_command();
        sc_dt::uint64    adr = trans.get_address();
        unsigned char*   ptr = trans.get_data_ptr();

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

        // Target must honor BEGIN_RESP/END_RESP exclusion rule
        // i.e. must not send BEGIN_RESP until receiving previous END_RESP or BEGIN_REQ
        if (response_in_progress)
        {
          // Target allows only two transactions in-flight
          if (next_response_pending)
            SC_REPORT_FATAL("TLM-2", "Attempt to have two pending responses in target");
          next_response_pending = &trans;
        }
        else
          send_response(trans);
        break;
      }
    }
  }

  void send_end_req(tlm::tlm_generic_payload& trans)
  {
    tlm::tlm_phase bw_phase;
    tlm::tlm_phase int_phase = internal_ph;
    sc_time delay;

    // Queue the acceptance and the response with the appropriate latency
    bw_phase = tlm::END_REQ;
    delay = sc_time(rand_ps(), SC_PS); // Accept delay
    socket->nb_transport_bw( trans, bw_phase, delay );
    // Ignore return value; initiator cannot terminate transaction at this point

    // Queue internal event to mark beginning of response
    delay = delay + sc_time(rand_ps(), SC_PS); // Latency
    m_peq.notify( trans, int_phase, delay );
    n_trans++;
  }

  void send_response(tlm::tlm_generic_payload& trans)
  {
    tlm::tlm_sync_enum status;
    tlm::tlm_phase bw_phase;
    sc_time delay;

    response_in_progress = true;
    bw_phase = tlm::BEGIN_RESP;
    delay = SC_ZERO_TIME;
    status = socket->nb_transport_bw( trans, bw_phase, delay );

    if (status == tlm::TLM_UPDATED)
    {
      // The timing annotation must be honored
      m_peq.notify( trans, bw_phase, delay);
    }
    else if (status == tlm::TLM_COMPLETED)
    {
      // The initiator has terminated the transaction
      trans.release();
      n_trans--;
      response_in_progress = false;

      // Target is now clear to unblock the initiator by issuing END_REQ
      if (end_req_pending)
      {
        send_end_req( *end_req_pending );
        end_req_pending = 0;
      }
    }
  }

  int   n_trans;
  bool  response_in_progress;
  tlm::tlm_generic_payload*  next_response_pending;
  tlm::tlm_generic_payload*  end_req_pending;
  tlm_utils::peq_with_cb_and_phase<AT_typeA_target> m_peq;
};

#endif

