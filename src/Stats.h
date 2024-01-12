/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2018 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the declaration of the statistics
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
 * refert to file ../doc/LICENSE_MCAERSIM.txt
 * 
 * 2022-10-06: Due to the different data type used for transmissions, some
 *             adaptations have been made with respect to naming and the parameters
 *             passed to methods. Also, methods getReceivedPackets() and
 *             getReceivedFlits() have been replaced by method getReceivedEvts(...).
 *             Furthermore, there have been changes to account for multiple
 *             processing elements per tile. Especially, this is facilitated by
 *             the additional attribute global_proc_ids. An additional method is
 *             provided, that can be used to obtain an irregular matrix of delays to
 *             reach a processing element within the tile. Since the body of method
 *             getCommunicationEnergy(..., ...) is commented out in Noxim, this 
 *             method has not been implemented in McAERsim. Finally, an attribute 
 *             and a method to monitor the number of local deliveries have been 
 *             included. This allows to check for consistency with the number of 
 *             received flits in case of source-address driven local multicast in 
 *             NENoCSi.
 *
 *///******************************************************************************** 

#ifndef __MCAERSIMSTATS_H__
#define __MCAERSIMSTATS_H__

#include <systemc.h>
#include <iostream>
#include <iomanip>
#include <vector>
#include <cassert>
#include "DataStructs.h"
#include "GlobalParams.h"

struct CommHistory
{
  int src_id;
  std::vector<double> delays;
  unsigned int total_received_evts;
  double last_received_evt_time;
};

// NOTE: searchCommHistory(...) always returns the index of a global processing element ID, aka src_id
class Stats
{
  int id;
  int global_proc_ids[NO_PES];
  std::vector<CommHistory> commhist[NO_PES];
  unsigned int local_deliveries;
  double warm_up_time;
  int searchCommHistory(int src_id, int proc_id);
 public:
  Stats(){}
  void configure(const int tile_id, const double _warm_up_time);
  // Access point for stats update
  void receivedEvt(const double arrival_time, const int proc_id, const AER_EVT& evt);
  // Returns the average delay (cycles) for the current processor element with
  // regard to the communication for which the source processor element is src_id
  double getAverageDelay(const int src_id, const int proc_id);
  // Returns the average delay (cycles) for the current processor element
  double getAverageDelay(const int proc_id);
  // Returns the maximum delay for the current processor element with regard
  // to the communication for which the source processor element is src_id
  double getMaxDelay(const int src_id, const int proc_id);
  //Returns the maximum delay (cycles) for the current processor element
  double getMaxDelay(const int proc_id);
  // Adds all delays of the local communication history to
  // the overall delays matrix passed as reference
  void setDelays(std::vector<std::vector<double> >& overall_delays) const;
  // Returns the average throughput (evts/cycle) for the current processor
  // element (PE) and for the communication for which the source PE is src_id
  double getAverageThroughput(const int src_id, const int proc_id);
  // Returns the average throughput (evts/cycle) for the current processor element
  double getAverageThroughput(const int proc_id);
  // Returns the number of received events from the current processor element
  unsigned int getReceivedEvts(const int proc_id);
  // Returns the number of locally delivered events
  unsigned int getLocalDeliveries() {return local_deliveries;};
  // Returns the number of communications for which the current processor
  // element is the destination node
  unsigned int getTotalCommunications(const int proc_id);
  // Shows statistics for the processing elements of a tile
  void showStats(std::ostream& out = std::cout, bool header = false);
};

#endif /* __MCAERSIMSTATS_H__ */
