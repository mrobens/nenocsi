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
 * 2022-09-01: Original creation of this file containing the declaration
 *             of the input arbiter
 *
 *///******************************************************************************** 

#ifndef __MCAERSIMARBITER_H__
#define __MCAERSIMARBITER_H__

#include <cassert>
#include <string>

class Arbiter {
  int granted_input;
  bool reserved;
 public:
  Arbiter();
  inline std::string name() const {return "Arbiter";}
  bool isAvailable() {return !reserved;};             // Check if output port is available
  int getGrant() {return granted_input;}              // Return the input that has been granted access to the TCAM
  void reserve(const int port_in);                    // Reserve output for given input. Asserts that output is available
  void release();                                     // Release output. Asserts that output is reserved.
};

#endif /* __MCAERSIMARBITER_H__ */
  
