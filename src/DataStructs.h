/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2018 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the declaration of the top-level of Noxim
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
 * 2022-09-11: McAERsim uses some data structures in common with Noxim, but a
 *             couple of different data structures as well. Accordingly,
 *             declarations were adapted / deleted. Additional data structures,
 *             PEconfig and PEconfigMap, were introduced as well. Also, enums
 *             have been adapted to account for the different router
 *             architecture. 
 *
 *///******************************************************************************** 

#ifndef __MCAERSIMDATASTRUCTS_H__
#define __MCAERSIMDATASTRUCTS_H__

#include <map>

// An AER event is charactierzed by source address and time stamp
struct AER_EVT {
  int neuron_id;               // Source neuron ID
  int src_id;                  // Global source address of processing element
  double timestamp;            // Time stamp
  int hop_no;                  // Current number of hops from source to destination

  // Constructors
  AER_EVT() {}
  AER_EVT(const int n, const int s, const double ts)
  {
    make(n, s, ts);
  }

  // Methods
  void make(const int n, const int s, const double ts)
  {
    neuron_id = n;
    src_id = s;
    timestamp = ts;
    hop_no = 0;
  }
  inline bool operator==(const AER_EVT& evt) const
  {
    return (evt.neuron_id == neuron_id && evt.src_id == src_id && evt.timestamp == timestamp && evt.hop_no == hop_no);
  }
};

// Coord -- XY coordinates of a tile in a mesh
struct Coord
{
  int x;                      // X coordinate
  int y;                      // Y coordinate
  inline bool operator==(const Coord& coord) const
  {
    return (coord.x == x && coord.y == y);
  }
};

typedef struct
{
  std::string spike_recorder;
  int first_neuron;
  int second_neuron;
} PEconfig;

typedef std::map<int, PEconfig> PEconfigMap;

typedef struct
{
  std::string label;
  double value;
} PowerBreakdownEntry;

enum
{
  BUFFER_PUSH_PWR_D,
  BUFFER_POP_PWR_D,
  BUFFER_FRONT_PWR_D,
  ARBITRATION_PWR_D,
  TCAM_LOOKUP_PWR_D,
  SRAM_LOOKUP_PWR_D,
  CROSSBAR_PWR_D,
  LINK_R2R_PWR_D,
  NI_PWR_D,
  NO_BREAKDOWN_ENTRIES_D
};

enum
{
  BUFFER_ROUTER_PWR_S,
  ARBITRATION_PWR_S,
  TCAM_PWR_S,
  SRAM_PWR_S,
  CROSSBAR_PWR_S,
  NI_PWR_S,
  NO_BREAKDOWN_ENTRIES_S
};

typedef struct
{
  int size;
  PowerBreakdownEntry breakdown[NO_BREAKDOWN_ENTRIES_D + NO_BREAKDOWN_ENTRIES_S];
} PowerBreakdown;
  
#endif /* __MCAERSIMDATASTRUCTS_H__ */
