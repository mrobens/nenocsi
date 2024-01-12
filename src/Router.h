/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2018 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the declaration of the router
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
 * 2022-11-02: McAERsim does not support different routing and selection functions
 *             as well as wireless connections. Respective attributes, methods and
 *             support functions have been removed. Also, there is no support for
 *             virtual channels and delta networks. A simulation can not be stopped
 *             based on drained volume. On the other hand, support for multiple
 *             processing elements per tile has been added as have been a function 
 *             and an attribute to check for look-up errors.
 *             A central aspect w.r.t. tree-based source-address driven multicast
 *             support is a different router pipeline, which requires different
 *             attributes and methods as compared to Noxim. Also, the different
 *             data type used for communication had to be considered. 
 *
 *///******************************************************************************** 

#ifndef __MCAERSIMROUTER_H__
#define __MCAERSIMROUTER_H__

#include <systemc.h>
#include "GlobalParams.h"
#include "DataStructs.h"
#include "GlobalRoutingTable.h"
#include "TCAM.h"
#include "SRAM.h"
#include "Buffer.h"
#include "Arbiter.h"
#include "Stats.h"
#include "Power.h"
#include "Utils.h"

SC_MODULE(Router)
{
  // I/O Ports
  sc_in_clk clock;                                            // The input clock for the router
  sc_in<bool> reset;                                          // The reset signal for the router

  sc_in<AER_EVT> evt_rx[RADIX];                               // The input channels
  sc_in<bool> req_rx[RADIX];                                  // Incoming request signals associated with the input channels
  sc_out<bool> ack_rx[RADIX];                                 // Outgoing acknowledge signals associated with the input channels
  sc_out<bool> buffer_full_status_rx[RADIX];                  // Outgoing buffer full status signals associated with the input channels

  sc_out<AER_EVT> evt_tx[RADIX];                              // The output signals
  sc_out<bool> req_tx[RADIX];                                 // Outgoing requeset signals associated with the output channels
  sc_in<bool> ack_tx[RADIX];                                  // Incoming acknowledge signals associated with the output channels
  sc_in<bool> buffer_full_status_tx[RADIX];                   // Incoming buffer full status signals associated with the output channels

  // Registers
  int local_id;                                               // Unique ID
  Buffer buffer[RADIX];                                       // Buffers associated with each input channel
  bool current_level_rx[RADIX];                               // Current level of the Alternating Bit Protocol (ABP) associated with the input channels
  bool current_level_tx[RADIX];                               // Current level ot the Alternating Bit Protocol (ABP) associated with the output channels

  // Additional registers used for pipeline stages
  int addr_from_tcam;
  outDirs o_dirs_from_sram;
  outDirs o_dirs_from_st;
  AER_EVT evt_from_arbiter;
  AER_EVT evt_from_tcam;
  AER_EVT evt_from_sram;
  AER_EVT evt_from_xbar;
  int grant_from_tcam;
  int grant_from_sram;
  int grant_from_xbar;
  bool en_pipe;                                               // Pipeline enable - set to false to realize back pressure

  Stats stats;                                                // Local statistics
  Power power;                                                // Power calculations
  LocalRT lrt;                                                // Routing table for this router
  Arbiter arbiter;                                            // Arbiter used to grant one input signal access the the TCAM
  TCAM tcam;                                                  // TCAM used for address look-up
  SRAM sram;                                                  // SRAM used to obtain output ports from address information
  unsigned long routed_evts;
  bool CAM_lookup_failure;                                    // Indicate if at least one CAM look-up failed

  // Methods
  void process();                                             // Method process calling the single pipeline stage functions
  void line_traversal();                                      // Function associated with the sixth pipeline stage
  void switch_traversal();                                    // Function associated with the fifth pipeline stage
  void sram_look_up();                                        // Function associated with the fourth pipeline stage
  void tcam_look_up();                                        // Function associated with the third pipeline stage
  void arbitration();                                         // Function associated with the second pipeline stage
  void buffer_write();                                        // Function associated with the first pipeline stage
  void perCycleUpdate();

  void configure(const int _id, const double _warm_up_time,
		 const unsigned int _max_buffer_size,
		 GlobalRoutingTable& grt);

  unsigned long getRoutedEvts();                              // Returns the number of routed AER_EVTs
  bool checkCamFailure() {return CAM_lookup_failure;}         // Return true if at least one CAM look-up failed
  void ShowBufferStats(std::ostream& out);

  // Constructor
  SC_CTOR(Router)
  {
    SC_METHOD(process);
    sensitive << reset;
    sensitive << clock.pos();

    SC_METHOD(perCycleUpdate);
    sensitive << reset;
    sensitive << clock.pos();
  }

 private:
  int start_from_port;                                        // Port from which to start the arbitration cycle
};

#endif /* __MCAERSIMROUTER_H__ */
