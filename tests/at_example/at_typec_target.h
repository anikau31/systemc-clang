
// Filename: at_typec_target.h

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


#ifndef __AT_TYPEC_TARGET_H__
#define __AT_TYPEC_TARGET_H__

#include "common_header.h"

//struct AT_typeC_target: sc_module
SC_MODULE(AT_typeC_target)
{
  // The typeC target does not return END_REQ at all, but sends BEGIN_RESP on the backward path

  tlm_utils::simple_target_socket<AT_typeC_target, 32> socket;

  SC_CTOR(AT_typeC_target)
  : socket("socket")
  , response_in_progress(false)
  , next_response_pending(0)
  , m_peq(this, &AT_typeC_target::peq_cb)
  {
    socket.register_nb_transport_fw(this, &AT_typeC_target::nb_transport_fw);
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
    {
      trans.acquire();

      tlm::tlm_phase int_phase = internal_ph;
      delay = sc_time(rand_ps(), SC_PS); // Latency

      // Queue internal event to mark beginning of response
      m_peq.notify( trans, int_phase, delay );
      break;
    }
    case tlm::END_RESP:
      // On receiving END_RESP, the target can release the transaction
      // and allow other pending transactions to proceed

      if (!response_in_progress)
        SC_REPORT_FATAL("TLM-2", "Illegal transaction phase END_RESP received by target");

      trans.release();

      // Target itself is now clear to issue the next BEGIN_RESP
      response_in_progress = false;
      if (next_response_pending)
      {
        send_response( *next_response_pending );
        next_response_pending = 0;
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
      response_in_progress = false;
    }
  }

  bool  response_in_progress;

  tlm::tlm_generic_payload*  next_response_pending;
  tlm_utils::peq_with_cb_and_phase<AT_typeC_target> m_peq;
};

#endif

