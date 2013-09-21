
// Filename: at_interconnect.h

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


#ifndef __AT_INTERCONNECT_H__
#define __AT_INTERCONNECT_H__

#include "common_header.h"

//struct AT_interconnect: sc_module
SC_MODULE(AT_interconnect)
{
  tlm_utils::multi_passthrough_target_socket   <AT_interconnect, 32> targ_socket;
  tlm_utils::multi_passthrough_initiator_socket<AT_interconnect, 32> init_socket;

  SC_CTOR(AT_interconnect)
  : targ_socket("targ_socket")
  , init_socket("init_socket")
  {
    targ_socket.register_b_transport              (this, &AT_interconnect::b_transport);
    targ_socket.register_nb_transport_fw          (this, &AT_interconnect::nb_transport_fw);
    targ_socket.register_get_direct_mem_ptr       (this, &AT_interconnect::get_direct_mem_ptr);
    targ_socket.register_transport_dbg            (this, &AT_interconnect::transport_dbg);
    init_socket.register_nb_transport_bw          (this, &AT_interconnect::nb_transport_bw);
    init_socket.register_invalidate_direct_mem_ptr(this, &AT_interconnect::invalidate_direct_mem_ptr);
  }

  void end_of_elaboration()
  {
    n_targets       = init_socket.size();
    n_inits         = targ_socket.size();
    req_in_progress = new gp_ptr  [n_targets];
    rsp_in_progress = new gp_ptr  [n_inits];
    end_req_event   = new sc_event[n_targets];
    end_rsp_event   = new sc_event[n_inits];
    req_queue       = new std::deque<Trans>[n_targets];
    rsp_queue       = new std::deque<Trans>[n_inits];

    // Spawn one process per target, to wake up on receiving END_REQ

    for (unsigned int i = 0; i < n_targets; i++)
    {
      sc_spawn_options* opt = new sc_spawn_options;
      opt->spawn_method();
      opt->dont_initialize();
      opt->set_sensitivity( &end_req_event[i] );

      ostringstream oss;
      oss << "end_req_process" << i;
      sc_spawn(sc_bind(&AT_interconnect::end_req_process, this, i), oss.str().c_str(), opt);

      req_in_progress[i] = 0;
    }

    // Spawn one process per initiator, to wake up on receiving END_RESP

    for (unsigned int i = 0; i < n_inits; i++)
    {
      sc_spawn_options* opt = new sc_spawn_options;
      opt->spawn_method();
      opt->dont_initialize();
      opt->set_sensitivity( &end_rsp_event[i] );

      ostringstream oss;
      oss << "end_rsp_process" << i;
      sc_spawn(sc_bind(&AT_interconnect::end_rsp_process, this, i), oss.str().c_str(), opt);

      rsp_in_progress[i] = 0;
    }
  }

  // Forward interface

  virtual void b_transport( int id, tlm::tlm_generic_payload& trans, sc_time& delay )
  {
    sc_dt::uint64 masked_address;
    unsigned int target = decode_address( trans.get_address(), masked_address );
    trans.set_address( masked_address );

    init_socket[target]->b_transport( trans, delay );
  }


  virtual tlm::tlm_sync_enum nb_transport_fw( int id, tlm::tlm_generic_payload& trans,
                                              tlm::tlm_phase& phase, sc_time& delay )
  {
    unsigned int target;

    // Sticky extension, survives the transaction pool
    route_extension* ext;
    accessor(trans).get_extension(ext);

    bool is_request = (phase == tlm::BEGIN_REQ);

    if (phase == tlm::BEGIN_REQ)
    {
      sc_dt::uint64 masked_address;
      target = decode_address( trans.get_address(), masked_address );
      trans.set_address( masked_address );

      if (!ext)
      {
        ext = new route_extension;
        accessor(trans).set_extension(ext);
      }
      ext->init   = id;
      ext->target = target;
      ext->dont_propagate_fw = false;

      if (req_in_progress[target])
      {
        // Queue the request to be sent forward later
        trans.acquire();
        Trans req;
        req.trans = &trans;
        req.time  = sc_time_stamp() + delay;
        req_queue[target].push_back( req );

        return tlm::TLM_ACCEPTED;
      }
      req_in_progress[target] = &trans;
    }
    else if (phase == tlm::END_RESP)
    {
      assert(ext);
      assert( ext->init == id );
      target = ext->target;

      assert( rsp_in_progress[ext->init] );
      end_rsp_event[ext->init].notify();

      if (ext->dont_propagate_fw)
        return tlm::TLM_ACCEPTED;
    }

    tlm::tlm_sync_enum status;
    status = init_socket[target]->nb_transport_fw( trans, phase, delay );

    if (status == tlm::TLM_UPDATED)
    {
      if ((phase == tlm::END_REQ) || (phase == tlm::BEGIN_RESP))
      {
        assert( req_in_progress[target] );
        end_req_event[target].notify();
      }

      // Need to check response exclusion rule here before passing BEGIN_RESP on return path!
      if (phase == tlm::BEGIN_RESP)
      {
        if (rsp_in_progress[id])
        {
          queue_the_rsp_to_be_sent_bw_later( id, &trans, delay );
          return tlm::TLM_ACCEPTED;
        }
        rsp_in_progress[id] = &trans;
      }
    }
    else if (status == tlm::TLM_COMPLETED)
    {
      // Incoming phase could have been BEGIN_REQ or END_RESP
      if ( is_request )
      {
        end_req_event[target].notify();

        // Once target has returned TLM_COMPLETED, do not propagate transaction forward again
        ext->dont_propagate_fw = true;

        // Need to check response exclusion rule here before passing BEGIN_RESP on return path!
        if (rsp_in_progress[id])
        {
          queue_the_rsp_to_be_sent_bw_later( id, &trans, delay );
          return tlm::TLM_ACCEPTED;
        }
      }
      // Cannot expect an END_RESP from the initiator after returning TLM_COMPLETED
    }

    return status;
  }

  virtual bool get_direct_mem_ptr( int id, tlm::tlm_generic_payload& trans,
                                           tlm::tlm_dmi& dmi_data)
  {
    sc_dt::uint64 masked_address;
    unsigned int target = decode_address( trans.get_address(), masked_address );
    trans.set_address( masked_address );

    bool status = init_socket[target]->get_direct_mem_ptr( trans, dmi_data );

    dmi_data.set_start_address( reconstruct_address(dmi_data.get_start_address(), target) );
    dmi_data.set_end_address(   reconstruct_address(dmi_data.get_end_address(), target) );

    return status;
  }

  virtual unsigned int transport_dbg( int id, tlm::tlm_generic_payload& trans )
  {
    sc_dt::uint64 masked_address;
    unsigned int target = decode_address( trans.get_address(), masked_address );
    trans.set_address( masked_address );

    return init_socket[target]->transport_dbg( trans );
  }


  // Backward interface

  virtual tlm::tlm_sync_enum nb_transport_bw( int id, tlm::tlm_generic_payload& trans,
                                              tlm::tlm_phase& phase, sc_time& delay )
  {
    if (phase == tlm::END_REQ || (&trans == req_in_progress[id] && phase == tlm::BEGIN_RESP))
    {
      assert( req_in_progress[id] );
      end_req_event[id].notify();
    }

    route_extension* ext;
    accessor(trans).get_extension(ext);
    assert(ext);

    int init   = ext->init;

    if (phase == tlm::BEGIN_RESP)
    {
      if (rsp_in_progress[init])
      {
        queue_the_rsp_to_be_sent_bw_later( init, &trans, delay );
        return tlm::TLM_ACCEPTED;
      }

      rsp_in_progress[init] = &trans;

      // Set the delay so there are multiple responses pending
      delay = delay + sc_time(1000, SC_PS);
    }

    tlm::tlm_sync_enum status;
    status = targ_socket[init]->nb_transport_bw( trans, phase, delay );

    if (status == tlm::TLM_COMPLETED)
    {
      assert( phase == tlm::BEGIN_RESP );
      assert( rsp_in_progress[init] );
      end_rsp_event[init].notify();
    }

    return status;
  }


  virtual void invalidate_direct_mem_ptr( int id, sc_dt::uint64 start_range,
                                                  sc_dt::uint64 end_range )
  {
    // Reconstruct address range in system memory map
    sc_dt::uint64 bw_start_range = reconstruct_address(start_range, id);
    sc_dt::uint64 bw_end_range   = reconstruct_address(end_range, id);

    // Propagate call backward to all initiators
    for (unsigned int i = 0; i < targ_socket.size(); i++)
      targ_socket[i]->invalidate_direct_mem_ptr(bw_start_range, bw_end_range);
  }


private:

  void end_req_process( unsigned int target )
  {
    // Called whenever END_REQ is received from a target
    assert( req_in_progress[target] );

    if ( req_queue[target].empty() )
    {
      req_in_progress[target] = 0;
    }
    else
    {
      // Requests are serviced in a first-come-first-served order
      // You would re-write this to implement the arbitration strategy of
      // your particular interconnect component.

      Trans req = req_queue[target].front();
      req_queue[target].pop_front();

      gp_ptr         trans = req.trans;
      tlm::tlm_phase phase = tlm::BEGIN_REQ;
      sc_time        delay;

      if (req.time < sc_time_stamp())
        delay = SC_ZERO_TIME;
      else
        delay = req.time - sc_time_stamp();

      req_in_progress[target] = trans;

      route_extension* ext;
      accessor(*trans).get_extension(ext);
      assert(ext);
      assert( ext->target == int(target) );
      int init = ext->init;

      tlm::tlm_sync_enum status;
      status = init_socket[target]->nb_transport_fw( *trans, phase, delay );

      if (status == tlm::TLM_UPDATED)
      {
        if ((phase == tlm::END_REQ) || (phase == tlm::BEGIN_RESP))
        {
          assert( req_in_progress[target] );
          end_req_event[target].notify();
        }

        // Need to check response exclusion rule
        if (phase == tlm::BEGIN_RESP)
        {
          if (rsp_in_progress[init])
          {
            queue_the_rsp_to_be_sent_bw_later( init, trans, delay );
            return;
          }
          rsp_in_progress[init] = trans;
        }

        // Need to propagate transaction to initiator on backward path
        send_bw_to_initiator( trans, phase, delay);
      }
      else if (status == tlm::TLM_COMPLETED)
      {
        // The original BEGIN_REQ was stalled because there was an outstanding request on the
        // target-side socket. Then on finally sending the delayed BEGIN_REQ to the target,
        // the target immediately returns TLM_COMPLETED. This suggests a weird target!

        assert( req_in_progress[target] );
        end_req_event[target].notify();

        // Once target has returned TLM_COMPLETED, do not propagate transaction forward again
        ext->dont_propagate_fw = true;

        // Need to propagate response to initiator on backward path
        phase = tlm::BEGIN_RESP;

        // Need to check response exclusion rule
        if (rsp_in_progress[init])
        {
          queue_the_rsp_to_be_sent_bw_later( init, trans, delay );
          return;
        }
        rsp_in_progress[init] = trans;

        // Need to propagate transaction to initiator on backward path
        send_bw_to_initiator( trans, phase, delay);
      }

      // Finally finished with the delayed request transaction
      trans->release();
    }
  }

  void queue_the_rsp_to_be_sent_bw_later( unsigned int init, tlm::tlm_generic_payload* trans, sc_time& delay )
  {
    trans->acquire();
    Trans rsp;
    rsp.trans = trans;
    rsp.time  = sc_time_stamp() + delay;
    rsp_queue[init].push_back( rsp );
  }

  void end_rsp_process( unsigned int init )
  {
    // Called whenever END_RESP (or TLM_COMPLETED) is received from an initiator
    assert( rsp_in_progress[init] );

    if ( rsp_queue[init].empty() )
    {
      rsp_in_progress[init] = 0;
    }
    else
    {
      Trans rsp = rsp_queue[init].front();
      rsp_queue[init].pop_front();

      gp_ptr         trans = rsp.trans;
      tlm::tlm_phase phase = tlm::BEGIN_RESP;
      sc_time        delay;

      if (rsp.time < sc_time_stamp())
        delay = SC_ZERO_TIME;
      else
        delay = rsp.time - sc_time_stamp();

      rsp_in_progress[init] = trans;

      // Need to propagate transaction to initiator on backward path
      send_bw_to_initiator( trans, phase, delay);

      // Finally finished with the delayed response transaction
      trans->release();
    }
  }

  void send_bw_to_initiator( tlm::tlm_generic_payload* trans,
                             tlm::tlm_phase& phase, sc_time& delay )
  {
    // Propagate END_REQ or BEGIN_RESP transaction to initiator on backward path

    route_extension* ext;
    accessor(*trans).get_extension(ext);
    assert(ext);
    int init   = ext->init;
    int target = ext->target;

    tlm::tlm_sync_enum status = targ_socket[init]->nb_transport_bw( *trans, phase, delay );

    if (status == tlm::TLM_UPDATED || status == tlm::TLM_COMPLETED)
    {
      // Initiator would normally return TLM_COMPLETED here
      // TLM_UPDATED only makes sense when an extended phase will be sent later

      assert( rsp_in_progress[init] );
      end_rsp_event[init].notify();

      if (ext->dont_propagate_fw)
        return;

      // Need to propagate END_RESP transaction to target on forward path
      phase = tlm::END_RESP;

      status = init_socket[target]->nb_transport_fw( *trans, phase, delay );
      assert( status != tlm::TLM_UPDATED );
      // Ignore return value
    }
  }

  unsigned int decode_address( sc_dt::uint64 address, sc_dt::uint64& masked_address )
  {
    // You would re-write this function to implement the particular routing strategy
    // of your interconnect component.

    // Use the top bits of the address to indicate target
    masked_address = address & 0xff;
    unsigned int target = (unsigned int)(address >> 8);

    // If address is too big, just wrap around the target number
    return target % n_targets;
  }

  sc_dt::uint64 reconstruct_address ( uint64 address, unsigned int target )
  {
    return address | (target << 8);
  }


private:

  unsigned int n_targets;
  unsigned int n_inits;

  typedef tlm::tlm_generic_payload* gp_ptr;

  // Extension to store routing information in transaction

  struct route_extension: tlm_utils::instance_specific_extension<route_extension>
  {
    int init;
    int target;
    bool dont_propagate_fw;
  };

  tlm_utils::instance_specific_extension_accessor accessor;

  // Flags, events and queues to implement the request and response exclusion rules

  gp_ptr*     req_in_progress;
  gp_ptr*     rsp_in_progress;
  sc_event*   end_req_event;
  sc_event*   end_rsp_event;

  struct Trans
  {
    gp_ptr  trans;
    sc_time time;
  };

  std::deque<Trans>* req_queue;
  std::deque<Trans>* rsp_queue;
};

#endif

