/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2018 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the declaration of the buffer
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
 * 2022-09-01: Class declaration has been re-organized
 *             Adaptations to account for a different data type
 *             (AER_EVT instead of Flit)
 *             A type definition and function declarations that are not required
 *             in McAERsim have been removed. 
 *
 *///******************************************************************************** 

#ifndef __MCAERSIMBUFFER_H__
#define __MCAERSIMBUFFER_H__

#include <cassert>
#include <queue>
#include <iomanip>
#include <systemc.h>
#include "DataStructs.h"
#include "GlobalParams.h"

class Buffer {
  std::string label;
  unsigned int max_buffer_size;
  bool true_buffer;
  std::queue <AER_EVT> buffer;
  // Variables required for deadlock checking
  int full_cycles_counter;
  AER_EVT last_AER_EVT;
  bool deadlock_detected;
  // Variables and methods required for statistical evaluation
  unsigned int max_occupancy;
  double hold_time, last_event, hold_time_sum;
  double mean_occupancy;
  int previous_occupancy;
  void SaveOccupancyAndTime();
  void UpdateMeanOccupancy();
 public:
  Buffer();                                         // Constructor
  virtual ~Buffer(){}                               // Destructor
  void setLabel(std::string);                       // Sets the buffer label
  std::string getLabel() const;                     // Returns the buffer label
  void SetMaxBufferSize(const unsigned int bms);    // Set maximum buffer size (in AER events)
  unsigned int GetMaxBufferSize() const;            // Get maximum buffer size (in AER events)
  unsigned int getCurrentFreeSlots() const;         // Free slots in the buffer
  bool IsFull() const;                              // Returns true if buffer is full
  bool IsEmpty() const;                             // Returns true if buffer is empty
  void Push(const AER_EVT& evt);                    // Push an AER event
  AER_EVT Pop();                                    // Pops an AER event
  AER_EVT Front() const;                            // Return a copy of the first AER event in the buffer
  unsigned int Size() const;                        // Returns the buffer size
  void ShowStats(std::ostream& out);                // Show statistical parameters
  void Disable();                                   // Sets true_buffer to false
  void Print();                                     // Displays the current buffer content
  bool deadlockFree();                              // True if AER event at the front is not in deadlock
  void deadlockCheck();                             // Checks if AER event at the front is in dealock
};

#endif /* __MCAERSIMBUFFER_H__ */
