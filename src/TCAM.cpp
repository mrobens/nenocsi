/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2018 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
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
 * 2022-09-07: Original creation of this file containing the implementation
 *             of the TCAM
 *
 *///********************************************************************************

#include "TCAM.h"

bool TCAM::init(GlobalRoutingTable& grt, const int id)
{
  assert(grt.isValid());                                                // Make sure GlobalRoutingTable was loaded before
  LocalRT lrt = grt.getLocalRoutingTable(id);                           // Get the local routing table for this node
  int addr = 0;
  for (LocalRT::const_iterator it = lrt.begin(); it != lrt.end(); ++it)
  {
    storage[it->first] = addr;                                          // Set TCAM content
    ++addr;
  }
  return true;
}
  
bool TCAM::look_up(const int pattern, int& o_addr)
{
  if (storage.find(pattern) == storage.end())
    return false;

  o_addr = storage[pattern];
  return true;
}
