
// Filename: at_typee_target.h

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


#ifndef __AT_TYPEE_TARGET_H__
#define __AT_TYPEE_TARGET_H__

#include "common_header.h"

//struct AT_typeE_target: sc_module
SC_MODULE(AT_typeE_target)
{
  // The typeE target immediately returns TLM_COMPLETED (implicit BEGIN_RESP)
  // It does not use the backward path at all, and does not expect to receive END_RESP

  tlm_utils::simple_target_socket<AT_typeE_target, 32> socket;

  SC_CTOR(AT_typeE_target)
  : socket("socket")
  {
    socket.register_nb_transport_fw(this, &AT_typeE_target::nb_transport_fw);
  }

  virtual tlm::tlm_sync_enum nb_transport_fw( tlm::tlm_generic_payload& trans,
                                              tlm::tlm_phase& phase, sc_time& delay )
  {
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
    return tlm::TLM_COMPLETED;
  }

};

#endif

