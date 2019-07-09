
// Filename: at_typeb_initiator.h

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


#ifndef __AT_TYPEB_INITIATOR_H__
#define __AT_TYPEB_INITIATOR_H__

#include "common_header.h"

//struct AT_typeB_initiator: sc_module
SC_MODULE(AT_typeB_initiator)
{
  // The typeB initiator returns TLM_COMPLETED on the return path after receiving BEGIN_RESP
  // It may still need to send END_RESP on the backward path if the target sends BEGIN_RESP
  // on the return path

  tlm_utils::simple_initiator_socket<AT_typeB_initiator, 32> socket;

  SC_CTOR(AT_typeB_initiator)
  : socket("socket")  // Construct and name socket
  , req_in_progress(0)
  , m_peq(this, &AT_typeB_initiator::peq_cb)
  {
    socket.register_nb_transport_bw(this, &AT_typeB_initiator::nb_transport_bw);

    SC_THREAD(thread_process);
  }

  void thread_process()
  {
    tlm::tlm_generic_payload* trans;
    tlm::tlm_phase phase;
    sc_time delay;

    // Generate a sequence of random transactions
    for (int i = 0; i < 1000; i++)
    {
      int adr = rand() & 0xFFFC; // Address is word-aligned

      tlm::tlm_command cmd = static_cast<tlm::tlm_command>(rand() % 2);
      if (cmd == tlm::TLM_WRITE_COMMAND) data[i % 16] = adr;

      // Grab a new transaction from the memory manager
      trans = m_mm.allocate();
      trans->acquire();

      trans->set_command( cmd );
      trans->set_address( adr );
      trans->set_data_ptr( reinterpret_cast<unsigned char*>(&data[i % 16]) );
      trans->set_data_length( 4 );
      trans->set_streaming_width( 4 );
      trans->set_byte_enable_ptr( 0 );
      trans->set_dmi_allowed( false );
      trans->set_response_status( tlm::TLM_INCOMPLETE_RESPONSE );

      // Initiator must honor BEGIN_REQ/END_REQ exclusion rule
      if (req_in_progress)
        wait(end_req_event);
      req_in_progress = trans;
      phase = tlm::BEGIN_REQ;

      // Timing annotation models processing time of initiator prior to call
      delay = sc_time(rand_ps(), SC_PS);

      fout << hex << adr << " new, cmd=" << (cmd ? "write" : "read")
           << ", data=" << hex << data[i % 16] << " at time " << sc_time_stamp()
           << " in " << name() << endl;

      // Non-blocking transport call on the forward path
      tlm::tlm_sync_enum status;
      status = socket->nb_transport_fw( *trans, phase, delay );

      // Check value returned from nb_transport_fw
      if (status == tlm::TLM_UPDATED)
      {
        // The timing annotation must be honored
        m_peq.notify( *trans, phase, delay );
      }
      else if (status == tlm::TLM_COMPLETED)
      {
        // The completion of the transaction necessarily ends the BEGIN_REQ phase
        req_in_progress = 0;

        // The target has terminated the transaction
        check_transaction( *trans );

        // Allow the memory manager to free the transaction object
        trans->release();
      }
      wait( sc_time(rand_ps(), SC_PS) );
    }
  }

  // TLM-2 backward non-blocking transport method

  virtual tlm::tlm_sync_enum nb_transport_bw( tlm::tlm_generic_payload& trans,
                                              tlm::tlm_phase& phase, sc_time& delay )
  {
    if (phase == tlm::END_REQ)
    {
      m_peq.notify( trans, phase, delay );
      return tlm::TLM_ACCEPTED;
    }
    else if (phase == tlm::BEGIN_RESP)
    {
      if ( &trans == req_in_progress )
      {
        // The end of the BEGIN_REQ phase
        req_in_progress = 0;
        end_req_event.notify();
      }

      check_transaction( trans );
      trans.release();
      delay = delay + sc_time(rand_ps(), SC_PS);
      return tlm::TLM_COMPLETED;
    }
    else
      SC_REPORT_FATAL("TLM-2", "Illegal transaction phase received by initiator");

    return tlm::TLM_ACCEPTED;
  }

  // Payload event queue callback to handle transactions from target
  // Transaction could have arrived through return path or backward path

  void peq_cb(tlm::tlm_generic_payload& trans, const tlm::tlm_phase& phase)
  {
    if (phase == tlm::END_REQ || (&trans == req_in_progress && phase == tlm::BEGIN_RESP))
    {
      // The end of the BEGIN_REQ phase
      req_in_progress = 0;
      end_req_event.notify();
    }
    else if (phase == tlm::BEGIN_REQ || phase == tlm::END_RESP)
      SC_REPORT_FATAL("TLM-2", "Illegal transaction phase received by initiator");

    if (phase == tlm::BEGIN_RESP)
    {
      check_transaction( trans );

      // Send final phase transition to target
      tlm::tlm_phase fw_phase = tlm::END_RESP;
      sc_time delay = sc_time(rand_ps(), SC_PS);
      socket->nb_transport_fw( trans, fw_phase, delay );
      // Ignore return value

      // Allow the memory manager to free the transaction object
      trans.release();
    }
  }

  // Called on receiving BEGIN_RESP or TLM_COMPLETED
  void check_transaction(tlm::tlm_generic_payload& trans)
  {
    if ( trans.is_response_error() )
    {
      char txt[100];
      sprintf(txt, "Transaction returned with error, response status = %s",
                   trans.get_response_string().c_str());
      SC_REPORT_ERROR("TLM-2", txt);
    }

    tlm::tlm_command cmd = trans.get_command();
    sc_dt::uint64    adr = trans.get_address();
    int*             ptr = reinterpret_cast<int*>( trans.get_data_ptr() );

    fout << hex << adr << " check, cmd=" << (cmd ? "write" : "read")
         << ", data=" << hex << *ptr << " at time " << sc_time_stamp()
         << " in " << name() << endl;

    if (cmd == tlm::TLM_READ_COMMAND)
      assert( *ptr == -int(adr) );
  }

  mm   m_mm;
  int  data[16];
  tlm::tlm_generic_payload* req_in_progress;
  sc_event end_req_event;
  tlm_utils::peq_with_cb_and_phase<AT_typeB_initiator> m_peq;
};

#endif

