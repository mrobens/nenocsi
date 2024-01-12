/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2018 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the declaration of the global params needed by Noxim
 * to forward configuration to every sub-block
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
 * 2022-09-10: Overloads of the output stream operator as well as the sc_trace
 *             function have been adapted for the different data type (AER_EVT
 *             instead of Flit). Since there is no support for virtual channels 
 *             as well as advanced selection strategies in McAERsim, respective
 *             overloads of the output stream operator as well as the sc_trace
 *             function have been removed. Similarly, auxiliary functions
 *             sameRadioHub(..., ...), hasRadioHub(...), and tile2Hub(...) have
 *             not been implemented, because there is no support for wireless
 *             connections. As there is also no support for delta topologies, 
 *             coordinate conversion functions could be simplified and auxiliary 
 *             function YouAreSwitch(...) has been removed.
 *
 *///******************************************************************************** 

#ifndef __MCAERSIMUTILS_H__
#define __MCAERSIMUTILS_H__

#include <systemc.h>
#include <iomanip>
#include <sstream>

#include "DataStructs.h"

#ifdef DEBUG
#define LOG (std::cout << std::setw(7) << std::left << sc_time_stamp().to_double() / GlobalParams::clock_period_ps << " " << name() << "::" << __func__ << "() --> ")
#else
template <class cT, class traits = std::char_traits<cT> >
class basic_nullbuf: public std::basic_streambuf<cT, traits>
{
  typename traits::int_type overflow(typename traits::int_type c)
  {
    return traits::not_eof(c);
  }
};
template <class cT, class traits = std::char_traits<cT> >
class basic_onullstream: public std::basic_ostream<cT, traits>
{
 public:
  basic_onullstream(): std::basic_ios<cT, traits>(&m_sbuf), std::basic_ostream<cT, traits>(&m_sbuf)
  {
    this->init(&m_sbuf);
  }
 private:
  basic_nullbuf<cT, traits> m_sbuf;
};
typedef basic_onullstream<char> onullstream;
typedef basic_onullstream<wchar_t> wonullsrteam;
static onullstream LOG;
#endif /* DEBUG */

// Output overloading

inline ostream& operator<<(ostream& os, const AER_EVT& evt)
{
  if (GlobalParams::verbose_mode == "VERBOSE_HIGH")
  {
    os << std::endl;
    os << "### AER_EVT ###" << std::endl;
    os << "Source Neuron [" << evt.neuron_id << "]" << std::endl;
    os << "Source Processing Element [" << evt.src_id << "]" << std::endl;
    os << "Timestamp at packet generation " << evt.timestamp << "ms" << std::endl;
    os << "Total number of hops from source to destination is " << evt.hop_no << std::endl;
  }
  else
  {
    os << "(Neuron " << evt.neuron_id << ", Src " << evt.src_id << ", Hops " << evt.hop_no << ")";
  }
  return os;
}

inline ostream& operator<<(ostream& os, const Coord& coord)
{
  os << "(" << coord.x << "," << coord.y << ")";
  return os;
}

// Trace overloading
// Note: For custom data types, that are used as data types for SystemC ports,
//       it is mandatory to overload the function sc_trace(...)

inline void sc_trace(sc_trace_file* &tf, const AER_EVT& evt, std::string& name)
{
  sc_trace(tf, evt.neuron_id, name + ".neuron_id");
  sc_trace(tf, evt.src_id, name + ".src_id");
  sc_trace(tf, evt.timestamp, name + ".timestamp");
  sc_trace(tf, evt.hop_no, name + ".hop_no");
}

// Miscellaneous common functions

inline int coord2Id(const Coord& coord)
{
  int id = (coord.y * GlobalParams::mesh_dim_x) + coord.x;
  assert(id < GlobalParams::mesh_dim_x * GlobalParams::mesh_dim_y);
  return id;
}

inline Coord id2Coord(int id)
{
  Coord coord;
  coord.x = id % GlobalParams::mesh_dim_x;
  coord.y = id / GlobalParams::mesh_dim_x;
  assert(coord.x < GlobalParams::mesh_dim_x);
  assert(coord.y < GlobalParams::mesh_dim_y);
  return coord;
}

inline void printMap(std::string label, const std::map<std::string, double>& m, std::ostream& out)
{
  out << label << " = [" << std::endl;
  for (std::map<std::string, double>::const_iterator i = m.begin(); i != m.end(); ++i)
    out << "\t" << std::scientific << i->second << "\t % " << i->first << std::endl;

  out << "];" << std::endl;
}

#endif /* __MCAERSIMUTILS_H__ */
