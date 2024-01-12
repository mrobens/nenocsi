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
 * 2022-09-06: Original creation of this file containing the declaration
 *             of the local event queue of each processing element
 *
 *///******************************************************************************** 

#ifndef __MCAERSIMLOCALEVENTQUEUE_H__
#define __MCAERSIMLOCALEVENTQUEUE_H__

#include <queue>
#include <utility>
#include <string>
#include <cassert>
#include <fstream>
#include "GlobalParams.h"

typedef std::pair<int, double> nest_spk_evt;

struct LocalEventQ
{
  LocalEventQ();
  bool init(const std::string& spk_rec_file, int ini_neuron, int fin_neuron);
  nest_spk_evt Front();
  void Pop();
  inline bool isValid() { return valid; }
  inline void invalidate() { valid = false; };
 private:
  std::queue<nest_spk_evt> l_evt_queue;
  bool valid;
};

#endif /* __MCAERSIMLOCALEVENTQUEUE_H__ */

