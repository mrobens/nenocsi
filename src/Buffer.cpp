/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2018 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the implementation of the buffer
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
 * 2022-09-09: Adaptations to account for a different data type
 *             (AER_EVT instead of Flit)
 *             Adaptations to account for two removed functions
 *             Small modifications in output formatting
 *
 *///******************************************************************************** 

#include "Buffer.h"

Buffer::Buffer(): last_AER_EVT(0,0,0.0)
{
  SetMaxBufferSize(GlobalParams::buffer_depth);
  true_buffer = true;
  // Deadlock detection (Reset)
  full_cycles_counter = 0;
  deadlock_detected = false;
  // Statistics evaluation (Reset)
  hold_time = 0.0;
  hold_time_sum = 0.0;
  last_event = 0.0;
  max_occupancy = 0;
  mean_occupancy = 0.0;
  previous_occupancy = 0;
}

void Buffer::setLabel(std::string l)
{
  label = l;
}

std::string Buffer::getLabel() const
{
  return label;
}

void Buffer::SetMaxBufferSize(const unsigned int bms)
{
  assert(bms > 0);
  max_buffer_size = bms;
}

unsigned int Buffer::GetMaxBufferSize() const
{
  return max_buffer_size;
}

unsigned int Buffer::getCurrentFreeSlots() const
{
  return (GetMaxBufferSize() - Size());
}

bool Buffer::IsFull() const
{
  return buffer.size() == max_buffer_size;
}

bool Buffer::IsEmpty() const
{
  return buffer.size() == 0;
}

void Buffer::Push(const AER_EVT& evt)
{
  SaveOccupancyAndTime();
  if (IsFull())
    assert(false);
  else
    buffer.push(evt);

  UpdateMeanOccupancy();
  if (max_occupancy < buffer.size())
    max_occupancy = buffer.size();
}

AER_EVT Buffer::Pop()
{
  AER_EVT evt;
  SaveOccupancyAndTime();
  if (IsEmpty())
    assert(false);
  else
  {
    evt = buffer.front();
    buffer.pop();
  }
  UpdateMeanOccupancy();
  return evt;
}

AER_EVT Buffer::Front() const
{
  AER_EVT evt;
  if (IsEmpty())
    assert(false);
  else
    evt = buffer.front();

  return evt;
}

unsigned int Buffer::Size() const
{
  return buffer.size();
}

void Buffer::ShowStats(std::ostream& out)
{
  if (true_buffer)
    out << std::setw(8) << std::left << std::fixed << std::setprecision(4) << mean_occupancy << std::setw(8) << max_occupancy;
  else
    out << std::setw(16) << "";
}

void Buffer::Disable()
{
  true_buffer = false;
}

void Buffer::Print()
{
  std::queue<AER_EVT> m = buffer;
  std::string bstr = "";
  std::cout << sc_time_stamp().to_double() / GlobalParams::clock_period_ps << "\t";
  std::cout << label << " QUEUE *[";
  while (!(m.empty()))
  {
    AER_EVT evt = m.front();
    m.pop();
    std::cout << bstr << evt.src_id << "|";
  }
  std::cout << "\b]*" << std::endl;
}

bool Buffer::deadlockFree()
{
  // Note: evt.src_id is the source neuron AER ID. Within a time step, it should be unique.
  //       Since assigment and comparison of AER events happens component-wise, the combination
  //       of time stamp and source neuron AER ID should be unique.
  //
  if (IsEmpty()) return true;
  AER_EVT evt = buffer.front();
  if (last_AER_EVT == evt)
  {
    full_cycles_counter++;
  }
  else
  {
    last_AER_EVT = evt;
    full_cycles_counter = 0;
  }
  if (full_cycles_counter > GlobalParams::dl_threshold)
  {
    return false;
  }
  return true;
}

void Buffer::deadlockCheck()
{
  // Note: evt.src_id is the source neuron AER ID. Within a time step, it should be unique.
  //       Since assigment and comparison of AER events happens component-wise, the combination
  //       of time stamp and source neuron AER ID should be unique.
  //
  if (IsEmpty()) return;
  AER_EVT evt = buffer.front();
  if (last_AER_EVT == evt)
  {
    full_cycles_counter++;
  }
  else
  {
    if (deadlock_detected)
    {
      std::cout << "WRONG DEADLOCK detection, please increase GlobalParams::dl_threshold! " << std::endl;
      assert(false);
    }
    last_AER_EVT = evt;
    full_cycles_counter = 0;
  }
  if (full_cycles_counter > GlobalParams::dl_threshold && !deadlock_detected)
  {
    double current_time = sc_time_stamp().to_double() / GlobalParams::clock_period_ps;
    std::cout << "WARNING: DEADLOCK DETECTED at cycle " << current_time << " in buffer: " << getLabel() << std::endl;
    deadlock_detected = true;
  }
}

void Buffer::SaveOccupancyAndTime()
{
  previous_occupancy = buffer.size();
  hold_time = (sc_time_stamp().to_double() / GlobalParams::clock_period_ps) - last_event;
  last_event = sc_time_stamp().to_double() / GlobalParams::clock_period_ps;
}

void Buffer::UpdateMeanOccupancy()
{
  double current_time = sc_time_stamp().to_double() / GlobalParams::clock_period_ps;
  if (current_time - GlobalParams::reset_time < GlobalParams::stats_warm_up_time)
    return;

  mean_occupancy = mean_occupancy * (hold_time_sum / (hold_time_sum + hold_time)) +
    (1.0 / (hold_time_sum + hold_time)) * hold_time * buffer.size();
  hold_time_sum += hold_time;
}
