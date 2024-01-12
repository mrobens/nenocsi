/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2018 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file represents the top-level testbench
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
 * 2022-09-08: This file has been simplified, since McAERsim doesn't support
 *             wireless networks, delta topologies, different traffic models
 *             or different selection strategies, yet. Also, it doesn't offer
 *             an ASCII monitor. Data types have been adapted and a pointer
 *             to a global neuron assignment table has been included. Since
 *             method buildCommon() would be largely simplified under the
 *             above conditions, remaining tasks are performed in the
 *             constructor instead.  
 *
 *///******************************************************************************** 

#ifndef __MCAERSIMNOC_H__
#define __MCAERSIMNOC_H__

#include <systemc.h>
#include "Tile.h"
#include "GlobalRoutingTable.h"
#include "GlobalNeuronAssignmentTables/GlobalNeuronAssignmentTable.h"
#include "GlobalNeuronAssignmentTables/GlobalNeuronAssignmentTables.h"

template <typename T>
struct sc_signal_NESW
{
  sc_signal<T> north;
  sc_signal<T> east;
  sc_signal<T> south;
  sc_signal<T> west;
};

SC_MODULE(NoC)
{
  // I/O ports
  sc_in_clk clock;                                                     // Clock signal for the NoC
  sc_in<bool> reset;                                                   // Reset signal for the NoC

  // Signals used in Mesh topology
  sc_signal_NESW<AER_EVT> **evt;
  sc_signal_NESW<bool> **req;
  sc_signal_NESW<bool> **ack;
  sc_signal_NESW<bool> **buffer_full_status;

  // Matrix of tiles
  Tile ***t;

  // Global tables
  GlobalRoutingTable grtable;
  GlobalNeuronAssignmentTable* gnat;

  // Constructor
  SC_CTOR(NoC)
  {
    // Ensure routing table availability
    assert(grtable.load(GlobalParams::routing_table_filename));
    // Ensure availability of global neuron assignment table
    gnat = GlobalNeuronAssignmentTables::get(GlobalParams::gnat_method);
    if (gnat == 0)
    {
      std::cerr << "FATAL: Invalid global neuron assignment table -gnat ";
      std::cerr << GlobalParams::gnat_method << ", check with mcaersim -help" << std::endl;
      exit(1);
    }
    assert(gnat->load(GlobalParams::gnat_string));
    
    if (GlobalParams::topology == "TOPOLOGY_MESH")
    {
      buildMesh();
    }
    else if (GlobalParams::topology == "TOPOLOGY_TORUS")
    {
      buildTorus();
    }
    else
    {
      std::cerr << "ERROR: Topology " << GlobalParams::topology << " is not yet supported." << std::endl;
      exit(0);
    }
  }

  // Support methods (used in GlobalStats.cpp)
  Tile *searchNode(const int id) const;

 private:
  void buildMesh();
  void buildTorus();
};

#endif /* __MCAERSIMNOC_H__ */
	
