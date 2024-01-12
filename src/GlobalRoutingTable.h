/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2018 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
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
 * 2022-09-01: This file is only loosely based on GlobalRoutingTable.h of Noxim.
 *             It uses the same methods, but the implementation of the routing
 *             table is different. For each node, it contains a map that assigns
 *             a set of output directions to a source neuron ID.
 *
 *///******************************************************************************** 

#ifndef __MCAERSIMGLOBALROUTINGTABLE_H__
#define __MCAERSIMGLOBALROUTINGTABLE_H__

#include <map>
#include <set>
#include <iostream>

// Output directions for the specific source neuron address
// within a given router. Since a set is used, uniqueness
// of the elements is guaranteed.
typedef std::set<int> outDirs;

// Local routing table of a specific node within the network.
// It associates a source neuron ID with a set of output
// directions.
typedef std::map<int, outDirs> LocalRT;

// Global routing table
// It contains a local routing table for each node in the
// network.
typedef std::map<int, LocalRT> GlobalRT;

struct GlobalRoutingTable
{
  // Constructor
  GlobalRoutingTable();
  // Methods
  bool load(const std::string& fname);
  LocalRT getLocalRoutingTable(const int node_id);
  bool isValid() { return valid;}
private:
  GlobalRT global_rt;
  bool valid;
};

#endif /* __MCAERSIMGLOBALROUTINGTABLE_H__ */
