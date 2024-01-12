/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2018 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the declaration of the tile
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
 * 2022-09-08: McAERsim does not support virtual channels, wireless connections, and 
 *             advanced selection functions. It uses a different data type for
 *             transmissions and adds support for multiple processing elements per 
 *             tile. The connectivity established in this file has been adapted 
 *             accordingly.
 *
 *///********************************************************************************

#ifndef __MCAERSIMTILE_H__
#define __MCAERSIMTILE_H__

#include <systemc.h>
#include "Router.h"
#include "ProcessingElement.h"

SC_MODULE(Tile)
{
  // I/O Ports
  sc_in_clk clock;                                               // Input clock for the tile
  sc_in<bool> reset;                                             // Reset signal for the tile

  sc_in<AER_EVT> evt_rx[DIRECTIONS];                             // Input channels
  sc_in<bool> req_rx[DIRECTIONS];                                // Incoming request signals associated with the input channels
  sc_out<bool> ack_rx[DIRECTIONS];                               // Outgoing acknowledge signals associated with the input channels
  sc_out<bool> buffer_full_status_rx[DIRECTIONS];                // Outgoing buffer full status signals associated with the input channels

  sc_out<AER_EVT> evt_tx[DIRECTIONS];                            // Output channels
  sc_out<bool> req_tx[DIRECTIONS];                               // Outgoing request signals associated with the output channels
  sc_in<bool> ack_tx[DIRECTIONS];                                // Incoming acknowledge signals associated with the output channels
  sc_in<bool> buffer_full_status_tx[DIRECTIONS];                 // Incoming buffer full status signals associated with the output channels

  // Signals required for router <-> PE connections
  sc_signal<AER_EVT> evt_rx_local[NO_PES];
  sc_signal<bool> req_rx_local[NO_PES];
  sc_signal<bool> ack_rx_local[NO_PES];
  sc_signal<bool> buffer_full_status_rx_local[NO_PES];

  sc_signal<AER_EVT> evt_tx_local[NO_PES];
  sc_signal<bool> req_tx_local[NO_PES];
  sc_signal<bool> ack_tx_local[NO_PES];
  sc_signal<bool> buffer_full_status_tx_local[NO_PES];


  // Registers
  int local_id;                                                  // Unique ID

  // Instances
  Router *r;                                                     // Router instance
  ProcessingElement *pe[NO_PES];                                 // Processing element instances

  // Constructor

  Tile(sc_module_name nm, int _id): sc_module(nm)
  {
    local_id = _id;

    // Router pin assignment
    r = new Router("Router");
    r->clock(clock);
    r->reset(reset);
    for (int i=0; i < DIRECTIONS; ++i)
    {
      r->evt_rx[i](evt_rx[i]);
      r->req_rx[i](req_rx[i]);
      r->ack_rx[i](ack_rx[i]);
      r->buffer_full_status_rx[i](buffer_full_status_rx[i]);

      r->evt_tx[i](evt_tx[i]);
      r->req_tx[i](req_tx[i]);
      r->ack_tx[i](ack_tx[i]);
      r->buffer_full_status_tx[i](buffer_full_status_tx[i]);
    }
    // Local connections
    for (int i=0; i < NO_PES; ++i)
    {
      r->evt_rx[DIRECTIONS + i](evt_tx_local[i]);
      r->req_rx[DIRECTIONS + i](req_tx_local[i]);
      r->ack_rx[DIRECTIONS + i](ack_tx_local[i]);
      r->buffer_full_status_rx[DIRECTIONS + i](buffer_full_status_tx_local[i]);

      r->evt_tx[DIRECTIONS + i](evt_rx_local[i]);
      r->req_tx[DIRECTIONS + i](req_rx_local[i]);
      r->ack_tx[DIRECTIONS + i](ack_rx_local[i]);
      r->buffer_full_status_tx[DIRECTIONS + i](buffer_full_status_rx_local[i]);
    }
    // Processing element assignment
    char pe_name[16];
    for (int i=0; i < NO_PES; ++i)
    {
      int j = local_id * NO_PES + i;
      sprintf(pe_name, "PE_%d", j);
      pe[i] = new ProcessingElement(pe_name);
      pe[i]->clock(clock);
      pe[i]->reset(reset);

      pe[i]->evt_rx(evt_rx_local[i]);
      pe[i]->req_rx(req_rx_local[i]);
      pe[i]->ack_rx(ack_rx_local[i]);
      pe[i]->buffer_full_status_rx(buffer_full_status_rx_local[i]);

      pe[i]->evt_tx(evt_tx_local[i]);
      pe[i]->req_tx(req_tx_local[i]);
      pe[i]->ack_tx(ack_tx_local[i]);
      pe[i]->buffer_full_status_tx(buffer_full_status_tx_local[i]);
    }
  } // Constructor
}; // SC_MODULE

#endif /* __MCAERSIMTILE_H__ */
