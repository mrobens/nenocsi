/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2018 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the declaration of the global statistics
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
 * 2022-10-06: To account for different data types, some method names have been 
 *             changed and few method declarations have been dropped. On the other 
 *             hand, additional method declarations have been added to check for 
 *             proper operation and report on additional statistics. Also, a means 
 *             to export result data is provided. 
 *
 *///******************************************************************************** 

#ifndef __MCAERSIMGLOBALSTATS_H__
#define __MCAERSIMGLOBALSTATS_H__

#include <iostream>
#include <iomanip>
#include <vector>
#include <boost/filesystem.hpp>

#include "NoC.h"
#include "Tile.h"

class GlobalStats
{
  const NoC* noc;
  std::vector<double> overall_delays;
  void updatePowerBreakdown(std::map<std::string, double>& dst, PowerBreakdown* src);
 public:
  GlobalStats(const NoC* _noc);
  // Indicates, if CAM look-up failed somewhere
  bool checkCamFailure();
  // Returns the aggregated average delay (cycles)
  double getAverageDelay();
  // Returns the aggregated average delay (cycles)
  // for communication scr_id->dst_id, where ID refers to
  // a global processing element ID
  double getAverageDelay(const int src_id, const int dst_id);
  // Returns the maximum delay (cycles)
  double getMaxDelay();
  // Returns the maximum delay (cycles) observed by destination dst_id.
  // Returns -1 if dst_id is not destination of any communication.
  double getMaxDelay(const int dst_id);
  // Returns the maximum delay (cycles) for communication src_id->dst_id
  double getMaxDelay(const int src_id, const int dst_id);
  // Retruns the matrix of maximum delays to reach any processing element of the network
  std::vector<std::vector<double> > getMaxDelayMtx();
  // Saves the delays observed by all processing elements in the network
  // in the private attribute overall_delays (vector of doubles)
  void setOverallDelays(std::vector<double>& overall_delays) const;
  // Returns the overall number of queued AER_EVTs
  long getOverallQueuedEvts() const;
  // Returns the aggregated average throughput (evts/cycle)
  // for communication src_id->dst_id, where IDs refer to
  // global processing element IDs
  double getAverageThroughput(const int src_id, const int dst_id);
  // Returns the aggregated average throughput (evts/cycle)
  double getAggregatedThroughput();
  // Returns the average throughput per processing element (evts/cycle/PE)
  double getThroughput();
  // Returns the average throughput considering only
  // active processing elements (evts/cycles/PE)
  double getActiveThroughput();
  // Returns the total number of received AER_EVTs
  unsigned int getReceivedEvts();
  // Returns the total number of local deliveries
  unsigned int getLocalDeliveries();
  // Returns the number of routed AER_EVTs for each router
  std::vector<std::vector<unsigned long> > getRoutedEvtsMtx();
  // Returns the total dynamic power
  double getDynamicPower();
  // Return the total static power
  double getStaticPower();
  // Returns the total power
  double getTotalPower() {return (getDynamicPower() + getStaticPower());}
  // Show global statistics
  void showStats(std::ostream& out = std::cout, bool detailed = false);
  void showBufferStats(std::ostream& out = std::cout);
  void showPowerBreakdown(std::ostream& out);
  // Exports selected statistics data, e.g. for Gnuplot processing
  bool exportStatData(void);
};

#endif /* __MCAERSIMGLOBALSTATS_H__ */
  
  
