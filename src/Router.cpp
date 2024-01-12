/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2018 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the implementation of the router
 */
/*\\\********************************************************************************
 * Downloaded March 23, 2022 from
 * https://github.com/davidepatti/noxim/tree/c52ebce2217e57bcd4ff11a97b400323bd00acd5
 ************************************************************************************
 *
 * McAERsim - NoC simulator with tree-based multicast support for AER packets
 * Modifications Copyright (C) 2022-2023 Forschungszentrum Juelich GmbH, ZEA-2
 * Author: Markus Robens <https://www.fz-juelich.de/profile/robens_m>
 * For the license applied to these modifications and McAERsim as a whole
 * refer to file ../doc/LICENSE_MCAERSIM.txt
 * 
 * 2022-11-02: There are quite some modifications in this file. Delta topologies,
 *             virtual channels, wireless connections, different selection and 
 *             routing functions as well as the termination of a simulation based 
 *             on drained volume are not supported in McAERsim, so that respective 
 *             methods and code segments have been removed. On the other hand, the 
 *             router pipeline has been extended and there is support for multiple 
 *             processing elements per tile. As imposed by tree-based multicast 
 *             routing, a packet may be forwarded to more than one output prior to 
 *             line traversal.
 *             Apart from modifications which account for the varied feature 
 *             support, the buffer_write() method of McAERsim aggrees with the 
 *             rxProcess() of Noxim. Likewise, the line_traversal() method leans 
 *             on the forwarding phase of method txProcess() in Noxim. Instead of 
 *             the reservation phase, however, new pipeline stages have been 
 *             introduced. Methods perCycleUpdate(), configure(...,...,...,...),
 *             getRoutedEvts(), and ShowBufferStats(...) also have been transfered
 *             with some modifications that account for the modified feature
 *             support and data types.
 *
 *///******************************************************************************** 

#include "Router.h"

void Router::process()
{
  line_traversal();
  switch_traversal();
  sram_look_up();
  tcam_look_up();
  arbitration();
  buffer_write();
}

void Router::buffer_write()
{
  if (reset.read())
  {
    for (int i = 0; i < RADIX; ++i)
    {
      current_level_rx[i] = 0;
      ack_rx[i].write(0);
      buffer_full_status_rx[i].write(false);
    }
    routed_evts = 0;
  }
  else
  {
    // This process simply sees a flow of incoming AER_EVTs.
    // All other actions are performed by other functions.
    for (int i = 0; i < RADIX; ++i)
    {
      /* To accept a new AER_EVT, the following conditions must match:
         1) there is an incoming request
         2) there is a free slot in the input buffer of direction i    */

      if (req_rx[i].read() == 1 - current_level_rx[i])
      {
	AER_EVT evt = evt_rx[i].read();
	if(!buffer[i].IsFull())
	{
	  // Store the incoming AER_EVT in the buffer
	  buffer[i].Push(evt);
	  power.bufferRouterPush();
	  LOG << "Input[" << i << "] collects AER EVT: " << evt << std::endl;

	  // Toggle the signal value of the Alternating Bit Protocol (ABP)
	  current_level_rx[i] = 1 - current_level_rx[i];
	  // If a new AER_EVT is injected from local PE
	  int src_router_id = evt.src_id / (int)NO_PES;
	  if (src_router_id == local_id)
	    power.networkInterface();
	}
	else  // buffer full
	{
	  // Should not happen due to buffer_full control signals
	  LOG << "Input[" << i << "] buffer full, AER_EVT: " << evt << std::endl;
	  assert(i > DIRECTION_WEST);
	}
      }
      ack_rx[i].write(current_level_rx[i]);
      // Updates buffer_full_status_rx control signals
      buffer_full_status_rx[i].write(buffer[i].IsFull());
    }
  }
}

void Router::arbitration()
{
  if (reset.read())
  {
    if (!arbiter.isAvailable())
    {
      arbiter.release();
    }
  }
  else  // Normal operation
  {
    if (en_pipe)
    {
      if (arbiter.isAvailable())
      {
        for (int j = 0; j < RADIX; ++j)
        {
	  int i = (start_from_port + j) % RADIX;
	  if (!buffer[i].IsEmpty())
	  {
	    arbiter.reserve(i);
	    evt_from_arbiter = buffer[i].Front();
	    power.bufferRouterFront();
	    buffer[i].Pop();
	    power.bufferRouterPop();                                    // Account for power used for buffer element removal
	    LOG << "Arbiter processed AER_EVT: " << evt_from_arbiter << std::endl;
	    break;
	  }
        }
        if (!arbiter.isAvailable())                                     // Reservation has been successful
        {
	  start_from_port = (start_from_port + 1) % RADIX;
	  power.arbitration();
        }
      }
    }
  }
}

void Router::tcam_look_up()
{
  if (reset.read())
  {
    addr_from_tcam = INVALID;
  }
  else
  {
    // Normal operation
    if (en_pipe)
    {
      if (!arbiter.isAvailable())                                       // Only perform address look-up if reservation is available
      {
        if (tcam.look_up(evt_from_arbiter.neuron_id, addr_from_tcam))
        {
	  power.tcam_lookup();
	  evt_from_tcam = evt_from_arbiter;
	  grant_from_tcam = arbiter.getGrant();
	  LOG << "CAM processed AER_EVT: " << evt_from_tcam << std::endl;
	  arbiter.release();
        }
        else
	{
	  CAM_lookup_failure = true;
	  LOG << " TCAM look-up failed ! " << std::endl;
	}
      }
    }
  }
}

void Router::sram_look_up()
{
  if (reset.read())
  {
    o_dirs_from_sram.clear();
  }
  else // Normal operation
  {
    if (en_pipe)
    {
      if (addr_from_tcam != INVALID)
      {
        if (sram.look_up(addr_from_tcam, o_dirs_from_sram))
	{
	  power.sram_lookup();
	  evt_from_sram = evt_from_tcam;
	  grant_from_sram = grant_from_tcam;
	  LOG << "SRAM processed AER_EVT: " << evt_from_sram << std::endl;
	  addr_from_tcam = INVALID;
	}
	else
	  LOG << " SRAM look-up failed! " << std::endl;        
      }
    }
  }
}

void Router::switch_traversal()
{
  if (reset.read())
  {
    o_dirs_from_st.clear();
  }
  else
  {
    // Normal operation
    if (en_pipe)
    {
      if (o_dirs_from_sram.size() > 0)
      {
        o_dirs_from_st = o_dirs_from_sram;
	power.crossbar(o_dirs_from_st.size());
	evt_from_xbar = evt_from_sram;
	grant_from_xbar = grant_from_sram;
	LOG << "Crossbar forwarded AER_EVT: " << evt_from_xbar << std::endl;
	o_dirs_from_sram.clear();
      }
    }
  }
}

void Router::line_traversal()
{
  if (reset.read())
  {
    // Clear outputs and indexes of transmission protocol
    for (int o = 0; o < RADIX; ++o)
    {
      current_level_tx[o] = 0;
      req_tx[o].write(0);
    }
    // Enable pipeline operation
    en_pipe = true;
  }
  else
  {
    if (o_dirs_from_st.size() > 0)
    {
      AER_EVT evt = evt_from_xbar;
      bool o_all_available = true;
      std::string o_ports = "";
      for (outDirs::const_iterator o_it = o_dirs_from_st.begin(); o_it != o_dirs_from_st.end(); ++o_it)
      {
	o_all_available = o_all_available && (current_level_tx[*o_it] == ack_tx[*o_it])
	  && (buffer_full_status_tx[*o_it].read() == false);
	o_ports += (std::to_string(*o_it) + ",");
      }
      o_ports += "\b]";
      if (o_all_available)
      {
	LOG << "Input[" << grant_from_xbar << "] forwardet to Outputs["
	    << o_ports << ", AER_EVT: " << evt << std::endl;
	evt.hop_no++; // Note: The final value has to be reduced by 1 due to transmission to the local PE
	for (outDirs::const_iterator o_it = o_dirs_from_st.begin(); o_it != o_dirs_from_st.end(); ++o_it)
	{
	  evt_tx[*o_it].write(evt);
	  current_level_tx[*o_it] = 1 - current_level_tx[*o_it];
	  req_tx[*o_it].write(current_level_tx[*o_it]);
	  power.r2rLink(); // Accounts for power used for link traversal (also for router to processing element traversal)
	  if (*o_it > DIRECTION_WEST)
	  {
	    LOG << " ProcessingElement[" << (*o_it - DIRECTIONS) << "] consumed AER_EVT: " << evt << std::endl;
	    int now = sc_time_stamp().to_double() / GlobalParams::clock_period_ps;
	    stats.receivedEvt(now, *o_it - DIRECTIONS, evt);
	    power.networkInterface();  // Accounts for power additionally consumed by the network interface
	  }
	  else if (grant_from_xbar < DIRECTIONS)      // Convert these two lines to comment to include DELIVERIES TO local processing
	    routed_evts++;                            // elements (PEs). DELIVERIES FROM local PEs are always neglected.  
	}
	//if (grant_from_xbar < DIRECTIONS)           // Uncomment these two lines to include DELIVERIES TO local processing
	//  routed_evts += o_dirs_from_st.size();     // elements (PEs). DELIVERIES FROM local PEs are always neglected.
	//
	// Clear last pipeline stage
	o_dirs_from_st.clear();
	// Advance pipeline
	en_pipe = true;
      }
      else
      {
	LOG << "Cannot forward Input[" << grant_from_xbar << "] to Outputs[" << o_ports << ", AER_EVT: " << evt << std::endl;
	// Stall pipeline
	en_pipe = false;
      }
    } // Matches "if (o_dirs_from_st.size() > 0)"
  }   // Matches "if (reset.read()) ... else"
}

void Router::perCycleUpdate()
{
  if(!reset.read())
  {
    power.leakageRouter();                           // Static power consumed only once in router 
    for (int i = 0; i < RADIX; ++i)
    {
      power.leakageBufferRouter();                   // Static power of all buffers associated with inputs
      if (i >= DIRECTIONS)
	power.leakageNetworkInterface();             // Static power of network interfaces from local PEs
    }
  }
}

void Router::configure(const int _id, const double _warm_up_time,
		       const unsigned int _max_buffer_size, GlobalRoutingTable& grt)
{
  local_id = _id;
  stats.configure(_id, _warm_up_time);
  start_from_port = DIRECTION_NORTH;
  CAM_lookup_failure = false;
  if (grt.isValid())
  {
    lrt = grt.getLocalRoutingTable(_id);
    tcam.init(grt, _id);
    sram.init(grt, _id);
  }
  for (int i=0; i < RADIX; ++i)
  {
    buffer[i].SetMaxBufferSize(_max_buffer_size);
    buffer[i].setLabel(std::string(name()) + "->buffer[" + std::to_string(i) + "]");
  }
  if (GlobalParams::topology == "TOPOLOGY_MESH")
  {
    int row = _id / GlobalParams::mesh_dim_x;
    int col = _id % GlobalParams::mesh_dim_x;
    if (row == 0)
      buffer[DIRECTION_NORTH].Disable();
    if (row == GlobalParams::mesh_dim_y - 1)
      buffer[DIRECTION_SOUTH].Disable();
    if (col == 0)
      buffer[DIRECTION_WEST].Disable();
    if (col == GlobalParams::mesh_dim_x - 1)
      buffer[DIRECTION_EAST].Disable();
  }
}

unsigned long Router::getRoutedEvts()
{
  return routed_evts;
}

void Router::ShowBufferStats(std::ostream &out)
{
  for (int i=0; i < RADIX; ++i)
  {
    buffer[i].ShowStats(out);
  }
  out << std::endl;
}
