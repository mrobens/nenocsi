/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2018 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the implementation of the global routing table
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
 * 2022-09-01: The init(...)-method of LocalEventQueue.cpp leans on the load(...)-
 *             method of Noxim's GlobalRoutingTable.cpp. Something similar applies
 *             to the constructor. The rest is tied to the functionality of the
 *             local event queue.
 *
 *///******************************************************************************** 

#include "LocalEventQueue.h"

LocalEventQ::LocalEventQ()
{
  valid = false;
}

bool LocalEventQ::init(const std::string& spk_rc_file, int ini_neuron, int fin_neuron)
{
  std::ifstream fin(spk_rc_file, std::ios::in);
  if (!fin)
    return false;

  bool stop = false;
  int skip_lines = 0;
  while (!fin.eof() && !stop)
  {
    char line[128];
    fin.getline(line, sizeof(line) -1);
    if (line[0] == '\0')
      stop = true;
    else
    {
      if (line[0] != '%')
      {
	if (skip_lines < GlobalParams::nest_spk_det_skip_lines)
	{
	  ++skip_lines;
	  continue;
	}
	int neuron_id;
	double spk_time;
	if (sscanf(line, "%d %lf", &neuron_id, &spk_time) == 2)
	{
	  if((neuron_id >= ini_neuron) && (neuron_id <= fin_neuron))
	  {
	    l_evt_queue.push(std::make_pair(neuron_id, (spk_time - GlobalParams::nest_t_presim)));
	  }
	}
	else
	  assert(false);
      }
    }
  }
  if (l_evt_queue.size() > 0)
    valid = true;
  return true;
}

nest_spk_evt LocalEventQ::Front()
{
  return l_evt_queue.front();
}

void LocalEventQ::Pop()
{
  l_evt_queue.pop();
  if (l_evt_queue.empty())
  {
    valid = false;
  }
}
