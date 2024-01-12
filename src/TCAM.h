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
 * 2022-09-05: Original creation of this file containing the declaration
 *             of the TCAM
 *
 *///******************************************************************************** 

#ifndef __MCAERSIMTCAM_H__
#define __MCAERSIMTCAM_H__

#include <map>
#include <set>
#include <cassert>

#include "GlobalRoutingTable.h"

// Note that only the look-up procedure is modeled for the TCAM.
// The write procedure for the TCAM is not modeled.

class TCAM
{
  std::map<int, int> storage;
 public:
  bool look_up(const int pattern, int& o_addr);
  bool init(GlobalRoutingTable& grt, const int id);
};

#endif /* __MCAERSIMTCAM_H__ */
