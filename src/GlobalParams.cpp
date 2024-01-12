/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2018 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the implementation of the command line parser
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
 * 2022-09-11: Since global parameters in GlobalParams.h are altered, their
 *             instantiation needs to be altered as well.
 *
 *///******************************************************************************** 

#include "GlobalParams.h"

// General configuration
std::string GlobalParams::config_filename;
std::string GlobalParams::power_config_filename;
// Topology parameters
std::string GlobalParams::topology;
int GlobalParams::mesh_dim_x;
int GlobalParams::mesh_dim_y;
int GlobalParams::proc_arr_dim_x;
int GlobalParams::proc_arr_dim_y;
int GlobalParams::neurons_per_node;
double GlobalParams::r2r_link_length;
// Buffer setup
int GlobalParams::evt_width;
int GlobalParams::buffer_depth;
int GlobalParams::dl_threshold;
// Power management
PowerConfig GlobalParams::power_configuration;
// Simulation setup
int GlobalParams::clock_period_ps;
int GlobalParams::reset_time;
int GlobalParams::stats_warm_up_time;
int GlobalParams::simulation_time;
int GlobalParams::rnd_generator_seed;
std::string GlobalParams::routing_table_filename;
// NEST related file parameters
double GlobalParams::nest_time_multiplier;
double GlobalParams::nest_t_presim;
int GlobalParams::nest_spk_det_skip_lines;
std::string GlobalParams::gnat_method;
std::string GlobalParams::gnat_string;
// Reporting
std::string GlobalParams::verbose_mode;
std::string GlobalParams::output_file_suffix;
bool GlobalParams::create_output_files;
bool GlobalParams::show_buffer_stats;
// Debugging
bool GlobalParams::detailed;
