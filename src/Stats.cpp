/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2018 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the implementation of the statistics
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
 * 2022-10-06: Most methods receive an additional parameter that is required to
 *             account for the multiple processing elements per tile support. Also,
 *             the different data type used for transmissions is reflected by some
 *             naming alterations as well as a different parameter passed to a
 *             function. The configure(..., ...) method needs to initialize the two 
 *             additional attributes local_deliveries and the global_proc_ids array. 
 *             In method receivedEvts(..., ..., ...) a conversion between NEST time 
 *             stamps and the time resolution is required. Also, the new attribute
 *             local_deliveries needs to be updated appropriately. Method
 *             getCommunicationEnergy(..., ...) has not been implemented, since its
 *             body is commented out in the Noxim version. On the other hand, method
 *             setDelays(...) has been added and returns an irregular matrix of delay
 *             values to reach the single processing elements of a tile via call by
 *             reference. Methods getReceivedPackets() and getReceivedFlits() have
 *             been replaced by method getReceivedEvts(...).
 *
 *///******************************************************************************** 

#include "Stats.h"

// Private methods

int Stats::searchCommHistory(int src_id, int proc_id)
{
  assert(proc_id < NO_PES);
  for (unsigned int i = 0; i < commhist[proc_id].size(); ++i)
    if (commhist[proc_id][i].src_id == src_id)
      return i;

  return -1;
}

// Public methods

void Stats::configure(const int node_id, const double _warm_up_time)
{
  id = node_id;
  for (int k = 0; k < NO_PES; ++k)
    global_proc_ids[k] = id * NO_PES + k;
  warm_up_time = _warm_up_time;
  local_deliveries = 0;
}

void Stats::receivedEvt(const double arrival_time, const int proc_id, const AER_EVT& evt)
{
  if (arrival_time - GlobalParams::reset_time < warm_up_time)
    return;

  int i = searchCommHistory(evt.src_id, proc_id);
  // First AER_EVT received from a given source
  // Initialize CommHist structure
  if (i == -1)
  {
    CommHistory ch;
    ch.src_id = evt.src_id;
    ch.total_received_evts = 0;
    commhist[proc_id].push_back(ch);
    i = commhist[proc_id].size() - 1;
  }
  // Note: In the following calculation, both the arrival_time and the second expression include reset_time as offset, so that it cancels out
  int delay = (int)arrival_time - (int)((evt.timestamp * GlobalParams::nest_time_multiplier) / (double)GlobalParams::clock_period_ps + GlobalParams::reset_time);
  commhist[proc_id][i].delays.push_back(delay);
  commhist[proc_id][i].total_received_evts++;
  commhist[proc_id][i].last_received_evt_time = arrival_time - GlobalParams::reset_time - warm_up_time;
  // Update local deliveries
  if (evt.src_id == global_proc_ids[proc_id])
    local_deliveries++;
}

double Stats::getAverageDelay(const int src_id, const int proc_id)
{
  double sum = 0.0;
  int i = searchCommHistory(src_id, proc_id);
  assert(i >= 0);
  for (unsigned int j = 0; j < commhist[proc_id][i].delays.size(); ++j)
    sum += commhist[proc_id][i].delays[j];

  return sum / (double) commhist[proc_id][i].delays.size();
}
  
double Stats::getAverageDelay(const int proc_id)
{
  double avg = 0.0;
  for (unsigned int k = 0; k < commhist[proc_id].size(); ++k)
  {
    unsigned int samples = commhist[proc_id][k].delays.size();
    if (samples)
      avg += (double) samples * getAverageDelay(commhist[proc_id][k].src_id, proc_id);
  }
  return avg / (double) getReceivedEvts(proc_id);
}

double Stats::getMaxDelay(const int src_id, const int proc_id)
{
  double maxd = -1.0;
  int i = searchCommHistory(src_id, proc_id);
  assert (i >= 0);
  for (unsigned int j = 0; j < commhist[proc_id][i].delays.size(); ++j)
    if (commhist[proc_id][i].delays[j] > maxd)
    {
      maxd = commhist[proc_id][i].delays[j];
    }

  return maxd;
}

double Stats::getMaxDelay(const int proc_id)
{
  double maxd = -1.0;
  for (unsigned int k = 0; k < commhist[proc_id].size(); ++k)
  {
    unsigned int samples = commhist[proc_id][k].delays.size();
    if (samples)
    {
      double m = getMaxDelay(commhist[proc_id][k].src_id, proc_id);
      if (m > maxd)
	maxd = m;
    }
  }
  return maxd;
}

void Stats::setDelays(std::vector<std::vector<double> >& overall_delays) const
{
  overall_delays.resize(NO_PES);
  for (unsigned int k = 0; k < NO_PES; ++k)
    for (unsigned int c = 0; c < commhist[k].size(); ++c)
      if (commhist[k][c].delays.size() > 0)
	overall_delays[k].insert(overall_delays[k].end(), commhist[k][c].delays.begin(), commhist[k][c].delays.end());
}

double Stats::getAverageThroughput(const int src_id, const int proc_id)
{
  int i = searchCommHistory(src_id, proc_id);
  assert (i >= 0);
  int current_sim_cycles = sc_time_stamp().to_double() / GlobalParams::clock_period_ps - warm_up_time - GlobalParams::reset_time;
  if (commhist[proc_id][i].total_received_evts == 0)
    return -1.0;
  else
    return (double) commhist[proc_id][i].total_received_evts / current_sim_cycles;
}

double Stats::getAverageThroughput(const int proc_id)
{
  double sum = 0.0;
  for (unsigned int k = 0; k < commhist[proc_id].size(); ++k)
  {
    double avg = getAverageThroughput(commhist[proc_id][k].src_id, proc_id);
    if (avg > 0.0)
      sum += avg;
  }
  return sum;
}

unsigned int Stats::getReceivedEvts(const int proc_id)
{
  int n = 0;
  for (unsigned int i = 0; i < commhist[proc_id].size(); ++i)
    n += commhist[proc_id][i].total_received_evts;

  return n;
}

unsigned int Stats::getTotalCommunications(const int proc_id)
{
  return commhist[proc_id].size();
}

void Stats::showStats(std::ostream& out, bool header)
{
  if (header)
  {
    out << "%"
        << std::setw(5) << "src"
        << std::setw(5) << "dst"
        << std::setw(10) << "delay avg"
        << std::setw(10) << "delay max"
        << std::setw(15) << "throughput"
        << std::setw(12) << "received" << std::endl;
    out << "%"
        << std::setw(5) << ""
        << std::setw(5) << ""
        << std::setw(10) << "cycles"
        << std::setw(10) << "cycles"
        << std::setw(15) << "evt/cycle"
        << std::setw(12) << "evts" << std::endl;
  }
  for (unsigned int k = 0; k < NO_PES; ++k)
  {
    for (unsigned int i = 0; i < commhist[k].size(); ++i)
    {
      out << " "
          << std::setw(5) << commhist[k][i].src_id
          << std::setw(5) << global_proc_ids[k]
          << std::setw(10) << getAverageDelay(commhist[k][i].src_id, k)
	  << std::setw(10) << getMaxDelay(commhist[k][i].src_id, k)
	  << std::setw(15) << getAverageThroughput(commhist[k][i].src_id, k)
	  << std::setw(12) << commhist[k][i].total_received_evts << std::endl;
    }
    out << "% Aggregated average delay (cycles): " << getAverageDelay(k) << std::endl;
    out << "% Aggregated average throughput (evts/cycle): " << getAverageThroughput(k) << std::endl;
  }
}
