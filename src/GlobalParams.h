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
 * 2022-09-11: McAERsim uses less preprocessor constants than Noxim.
 *             Also, there is no support for virtual channels or wireless
 *             connections. Respective structs and preprocessor defines are 
 *             thus removed. Preprocessor constants representing directions 
 *             are replaced by an enum. Struct RouterPowerConfig is adapted 
 *             to reflect the modified router architecture. The set of global 
 *             parameters is modified accordingly. 
 *
 *///******************************************************************************** 

#ifndef __MCAERSIMGLOBALPARAMS_H__
#define __MCAERSIMGLOBALPARAMS_H__

#include <map>
#include <string>
#include "DataStructs.h"
#include <boost/tuple/tuple_comparison.hpp>

#ifndef NO_PES
#define NO_PES 1
#endif /* NO_PES */
#define INVALID -1

typedef struct
{
  std::map<std::pair<int, int>, double> front;
  std::map<std::pair<int, int>, double> pop;
  std::map<std::pair<int, int>, double> push;
  std::map<std::pair<int, int>, double> leakage;
} BufferPowerConfig;

typedef std::map<double, std::pair<double, double> > LinkBitLinePowerConfig;

typedef struct
{
  std::map<boost::tuples::tuple<int, int, int>, std::pair<double, double> > crossbar_pm;
  std::map<int, std::pair<double, double> > network_interface;
  std::map<int, std::pair<double, double> > arbitration_pm;
  std::map<int, std::pair<double, double> > tcam_pm;
  std::map<int, std::pair<double, double> > sram_pm;
} RouterPowerConfig;

typedef struct
{
  BufferPowerConfig bufferPowerConfig;
  LinkBitLinePowerConfig linkBitLinePowerConfig;
  RouterPowerConfig routerPowerConfig;
} PowerConfig;

struct GlobalParams {
  // General configuration
  static std::string config_filename;
  static std::string power_config_filename;
  // Topology parameters
  static std::string topology;
  static int mesh_dim_x;
  static int mesh_dim_y;
  static int proc_arr_dim_x;
  static int proc_arr_dim_y;
  static int neurons_per_node;
  static double r2r_link_length;
  // Buffer stetup
  static int evt_width;
  static int buffer_depth;
  static int dl_threshold;
  // Power management
  static PowerConfig power_configuration;
  // Simulation setup
  static int clock_period_ps;
  static int reset_time;
  static int stats_warm_up_time;
  static int simulation_time;
  static int rnd_generator_seed;
  static std::string routing_table_filename;
  // NEST related file parameters
  static double nest_time_multiplier;
  static double nest_t_presim;
  static int nest_spk_det_skip_lines;
  static std::string gnat_method;
  static std::string gnat_string;
  // Reporting
  static std::string verbose_mode;
  static std::string output_file_suffix;
  static bool create_output_files;
  static bool show_buffer_stats;
  // Debugging
  static bool detailed;
  // Expendable definitions
  //static std::map<int, PEconfig> pe_configuration;
};

enum
{
 DIRECTION_NORTH,
 DIRECTION_EAST,
 DIRECTION_SOUTH,
 DIRECTION_WEST,
 DIRECTIONS
};

#define RADIX (DIRECTIONS + NO_PES)
#define CONFIG_FILENAME "config.yaml"
#define POWER_CONFIG_FILENAME "power.yaml"

#endif /* __MCAERSIMGLOBALPARAMS_H__ */
