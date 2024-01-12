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
 * 2022-10-10: Several adaptations to account for the different set of global
 *             parameters used by McAERsim as compared to Noxim 
 *
 *///******************************************************************************** 

#include "ConfigurationManager.h"

YAML::Node config;
YAML::Node power_config;

void loadConfiguration()
{
  std::cout << "Loading configuration from file \"" << GlobalParams::config_filename << "\"...";
  try
  {
    config = YAML::LoadFile(GlobalParams::config_filename);
    std::cout << " Done" << std::endl;
  }
  catch (YAML::BadFile &e)
  {
    std::cout << " Failed" << std::endl;
    std::cerr << "The specified YAML configuration file was not found!" << std::endl;
    exit(0);
  }
  catch (YAML::ParserException &pe)
  {
    std::cout << " Failed" << std::endl;
    std::cerr << "ERROR at line " << pe.mark.line + 1 << " column " << pe.mark.column + 1;
    std::cerr << ": "<< pe.msg << ". Please check indentation." << std::endl;
    exit(0);
  }

  std::cout << "Loading power configuration from file \"" << GlobalParams::power_config_filename << "\"...";
  try
  {
    power_config = YAML::LoadFile(GlobalParams::power_config_filename);
    std::cout << "Done" << std::endl;
  }
  catch (YAML::BadFile &e)
  {
    std::cout << " Failed" << std::endl;
    std::cerr << "The specified YAML power configuration file was not found!" << std::endl;
    exit(0);
  }
  catch (YAML::ParserException &pe)
  {
    std::cout << " Failed" << std::endl;
    std::cerr << "ERROR at line " << pe.mark.line + 1 << " column " << pe.mark.column + 1;
    std::cerr << ": " << pe.msg << ". Please check indentation." << std::endl;
    exit(0);
  }

  // Initialize global configuration parameters
  // (Can be overridden with command line arguments)
  //
  // Topology paramters
  GlobalParams::topology = readParam<std::string>(config, "topology", "TOPOLOGY_MESH");
  GlobalParams::mesh_dim_x = readParam<int>(config, "mesh_dim_x");
  GlobalParams::mesh_dim_y = readParam<int>(config, "mesh_dim_y");
  GlobalParams::proc_arr_dim_x = readParam<int>(config, "proc_arr_dim_x", 1);
  GlobalParams::proc_arr_dim_y = readParam<int>(config, "proc_arr_dim_y", 1);
  GlobalParams::neurons_per_node = readParam<int>(config, "neurons_per_node", 100);
  GlobalParams::r2r_link_length = readParam<double>(config, "r2r_link_length", 0.5);
  // Buffer setup
  GlobalParams::evt_width = readParam<int>(config, "evt_width", 32);
  GlobalParams::buffer_depth = readParam<int>(config, "buffer_depth", 8);
  GlobalParams::dl_threshold = readParam<int>(config, "deadlock_threshold", 50000);
  // Simulation setup
  GlobalParams::clock_period_ps = readParam<int>(config, "clock_period_ps", 1000);
  GlobalParams::reset_time = readParam<int>(config, "reset_time", 1000);
  GlobalParams::stats_warm_up_time = readParam<int>(config, "stats_warm_up_time", 0);
  GlobalParams::simulation_time = readParam<int>(config, "simulation_time");
  GlobalParams::rnd_generator_seed = readParam<int>(config, "rnd_generator_seed", 0);
  GlobalParams::routing_table_filename = readParam<std::string>(config, "routing_table_filename");
  // NEST related file parameters
  GlobalParams::nest_time_multiplier = readParam<double>(config, "nest_time_multiplier", 1e9);
  GlobalParams::nest_t_presim = readParam<double>(config, "nest_t_presim", 500.0);
  GlobalParams::nest_spk_det_skip_lines = readParam<int>(config, "nest_spk_det_skip_lines", 3);
  GlobalParams::gnat_method = readParam<std::string>(config, "gnat_method");
  GlobalParams::gnat_string = readParam<std::string>(config, "gnat_string");
  // Reporting
  GlobalParams::verbose_mode = readParam<std::string>(config, "verbose_mode", "VERBOSE_LOW");
  GlobalParams::show_buffer_stats = readParam<bool>(config, "show_buffer_stats", false);
  GlobalParams::create_output_files = readParam<bool>(config, "create_output_files", false);
  if (GlobalParams::create_output_files)
    GlobalParams::output_file_suffix = readParam<std::string>(config, "output_file_suffix");
  // Debugging
  GlobalParams::detailed = readParam<bool>(config, "detailed", false);
  // Power mangement
  GlobalParams::power_configuration = power_config["Energy"].as<PowerConfig>();
}

void showHelp(char selfname[])
{
  std::cout << std::endl;
  std::cout << "Usage: " << selfname << " [options]" << std::endl
            << "Where [options] is one or more of the following: " << std::endl
            << std::endl
            << "\t-help\t\tShow this help and exit" << std::endl
            << "\t-config S\tLoad the specified configuration file" << std::endl
            << "\t-pwr_conf S\tLoad the specified power configuration file" << std::endl
            << "\t-topology S\tSet the topology to one of the following: " << std::endl
            << "\t\t\tMESH\t2D Mesh" << std::endl
            << "\t\t\tTORUS\t2D Torus Mesh" << std::endl
            << "\t-dimX N\t\tSet the mesh X dimension" << std::endl
            << "\t-dimY N\t\tSet the mesh Y dimension" << std::endl
            << "\t-p_dimX N\tSet the local processor array X dimension" << std::endl
            << "\t-p_dimY N\tSet the local processor array Y dimension" << std::endl
            << "\t-npn N\t\tSet the number of neurons per node" << std::endl
            << "\t-r2r_ll D\tSet the router to router link length" << std::endl
            << "\t-evt_w N\tSet the data width of events" << std::endl
            << "\t-buffer N\tSet the depth of the router input buffers" << std::endl
            << "\t-threshold N\tSet the value used as deadlock threshold" << std::endl
            << "\t-period N\tSet the clock period to N picoseconds" << std::endl
            << "\t-reset N\tReset for N cycles" << std::endl
            << "\t-warmup N\tStart to collect statistics after N cycles" << std::endl
            << "\t-sim N\t\tRun the simulation for N cycles" << std::endl
            << "\t-rnd_seed N\tSet random generator seed to N" << std::endl
            << "\t-rt_file S\tFilename of the routing table that is to be used" << std::endl
            << "\t-nest_t_mul D\tMultiplier used to convert NEST time stamps" << std::endl
            << "\t\t\tto McAERsim time stamps (modify only if required)" << std::endl
	    << "\t-nest_presim D\tPre-simulation time used in NEST (in ms)" << std::endl
            << "\t-nest_skip N\tSkip N lines in NEST's spike recorder files" << std::endl
            << "\t-gnat S\t\tString indicating the global neuron assingment table method used" << std::endl
            << "\t\t\tCurrently supported values are \"gnat_seq\" and \"gnat_file\"" << std::endl
            << "\t-gnat_str S\tString needed to load the global neuron assignment table" << std::endl
            << "\t\t\tFor gnat_seq, these are two filenames connected by the $ sign," << std::endl
            << "\t\t\tsee file config.yaml for details." << std::endl
            << "\t-verbose S\tVerbosity level, where S is one of the following strings:" << std::endl
            << "\t\t\tVERBOSE_LOW" << std::endl
            << "\t\t\tVERBOSE_HIGH" << std::endl
            << "\t-of_suffix S\tSuffix S attached to the output files. Global parameter" << std::endl
            << "\t\t\tcreate_output_files will be set to true, if specified" << std::endl
            << "\t-buff_stat\tBuffer statistics will be shown if switch is present" << std::endl
            << "\t-detailed\tSwitch to create detailed output (for debugging)" << std::endl
            << std::endl
            << "In this table, single captial letters have the following meaning:" << std::endl
            << "N = integer, D = double, S = string" << std::endl
            << "At least a configuration file should be specified on the command line." << std::endl;
          //<< std::endl
  std::cout << std::endl;
}

void showConfig()
{
  std::cout << "Using the following configuration: " << std::endl
            << "- topology = " << GlobalParams::topology << std::endl
            << "- mesh_dim_x = " << GlobalParams::mesh_dim_x << std::endl
            << "- mesh_dim_y = " << GlobalParams::mesh_dim_y << std::endl
            << "- proc_arr_dim_x = " << GlobalParams::proc_arr_dim_x << std::endl
            << "- proc_arr_dim_y = " << GlobalParams::proc_arr_dim_y << std::endl
            << "- neurons_per_node = " << GlobalParams::neurons_per_node << std::endl
            << "- r2r_link_length = " << GlobalParams::r2r_link_length << std::endl
            << "- evt_width = " << GlobalParams::evt_width << std::endl
            << "- buffer_depth = " << GlobalParams::buffer_depth << std::endl
            << "- deadlock_threshold = " << GlobalParams::dl_threshold << std::endl
            << "- clock_period = " << GlobalParams::clock_period_ps << "ps" << std::endl
            << "- reset_time = " << GlobalParams::reset_time << " cycles" << std::endl
            << "- stats_warmup_time = " << GlobalParams::stats_warm_up_time << " cycles" << std::endl
            << "- simulation time = " << GlobalParams::simulation_time << " cycles" << std::endl
            << "- rnd_generator_seed = " << GlobalParams::rnd_generator_seed << std::endl
            << "- routing_table_filename = " << GlobalParams::routing_table_filename << std::endl
            << "- nest_time_multiplier = " << GlobalParams::nest_time_multiplier << std::endl
            << "- nest_t_presim = " << GlobalParams::nest_t_presim << "ms" << std::endl
            << "- nest_skip_lines = " << GlobalParams::nest_spk_det_skip_lines << std::endl
            << "- gnat_method = " << GlobalParams::gnat_method << std::endl
            << "- gnat_string = " << GlobalParams::gnat_string << std::endl
            << "- verbose = " << GlobalParams::verbose_mode << std::endl
            << "- output_file_suffix = " << GlobalParams::output_file_suffix << std::endl
            << "- show_buffer_stats = " << GlobalParams::show_buffer_stats << std::endl
            << "- detailed = " << GlobalParams::detailed << std::endl;
}

void checkConfiguration()
{
  if ((GlobalParams::topology.compare("TOPOLOGY_MESH") != 0) &&
      (GlobalParams::topology.compare("TOPOLOGY_TORUS") != 0))
  {
    std::cerr << "ERROR: Currently, only TOPOLOGY_MESH and TOPOLOGY_TORUS ";
    std::cerr << "are supported as topology string values" << std::endl;
    exit(1);
  }
  if (GlobalParams::mesh_dim_x <= 1)
  {
    std::cerr << "ERROR: dimX must be larger than 1" << std::endl;
    exit(1);
  }
  if (GlobalParams::mesh_dim_y <= 1)
  {
    std::cerr << "ERROR: dimY must be larger than 1" << std::endl;
    exit(1);
  }
  if (GlobalParams::proc_arr_dim_x < 1)
  {
    std::cerr << "ERROR: p_dimX must be larger than 0" << std::endl;
    exit(1);
  }
  if (GlobalParams::proc_arr_dim_y < 1)
  {
    std::cerr << "ERROR: p_dimY must be larger than 0" << std::endl;
    exit(1);
  }
  if (GlobalParams::proc_arr_dim_x * GlobalParams::proc_arr_dim_y != NO_PES)
  {
    std::cerr << "ERROR: The product of p_dimX and p_dimY does not match NO_PES" << std::endl;
    exit(1);
  }
  if (GlobalParams::neurons_per_node < 1)
  {
    std::cerr << "ERROR: Number of neurons per node must be larger than 0" << std::endl;
    exit(1);
  }
  if (GlobalParams::r2r_link_length < 0)
  {
    std::cerr << "ERROR: Router to router link length must be positive" << std::endl;
    exit(1);
  }
  if (GlobalParams::evt_width < 1)
  {
    std::cerr << "ERROR: Data width of AER_EVTs must be at least 1 bit" << std::endl;
    exit(1);
  }
  if (GlobalParams::buffer_depth < 1)
  {
    std::cerr << "ERROR: Buffer depth must be at least 1" << std::endl;
    exit(1);
  }
  if (GlobalParams::dl_threshold < 1000)
  {
    std::cerr << "ERROR: Deadlock threshold must be set large enough" << std::endl;
    exit(1);
  }
  if (GlobalParams::clock_period_ps < 1)
  {
    std::cerr << "ERROR: Clock period must be at least 1ps" << std::endl;
    exit(1);
  }
  if (GlobalParams::reset_time < 0)
  {
    std::cerr << "ERROR: Reset time must be larger or equal Zero" << std::endl;
    exit(1);
  }
  if (GlobalParams::stats_warm_up_time < 0)
  {
    std::cerr << "ERROR: Warm-up time for statistics needs to be larger or equal Zero" << std::endl;
    exit(1);
  }
  if (GlobalParams::simulation_time < GlobalParams::stats_warm_up_time)
  {
    std::cerr << "ERROR: Simulation time must be larger or equal to the statistics warm-up time" << std::endl;
    exit(1);
  }
  if (GlobalParams::rnd_generator_seed < 0)
  {
    std::cerr << "ERROR: Random number generator seed must be positive or be equal to Zero" << std::endl;
    exit(1);
  }
  if (GlobalParams::nest_time_multiplier < 1)
  {
    std::cerr << "ERROR: Time multiplier for NEST time stamp conversion must be larger or equal 1" << std::endl;
    exit(1);
  }
  if (GlobalParams::nest_t_presim < 0)
  {
    std::cerr << "ERROR: Pre-simulation time used in NEST must be larger or equal Zero" << std::endl;
    exit(1);
  }
  if (GlobalParams::nest_spk_det_skip_lines < 0)
  {
    std::cerr << "ERROR: Number of lines skipped in the spike recorder files of NEST must be larger or equal Zero" << std::endl;
    exit(1);
  }
  if ((GlobalParams::gnat_method.compare("gnat_seq") !=0)&&(GlobalParams::gnat_method.compare("gnat_file") !=0))
  {
    std::cerr << "ERROR: Wrong global neuron assigment table method specified." << std::endl;
    exit(1);
  }
  if ((GlobalParams::verbose_mode.compare("VERBOSE_LOW") != 0) &&
      (GlobalParams::verbose_mode.compare("VERBOSE_HIGH") != 0))
  {
    std::cerr << "ERROR: Only VERBOSE_LOW and VERBOSE_HIGH are supported verbosity levels" << std::endl;
    exit(1);
  }
}

void parseCmdLine(int argc, char* argv[])
{
  if (argc == 1)
    showHelp(argv[0]);
  else
  {
    for (int i = 1; i < argc; ++i)
    {
      if (!strcmp(argv[i], "-topology"))
      { 
	if (!strcmp(argv[++i], "MESH"))
	  GlobalParams::topology = "TOPOLOGY_MESH";
	else if (!strcmp(argv[i], "TORUS"))
	  GlobalParams::topology = "TOPOLOGY_TORUS";
	else
	{
	  std::cerr << "Unknown topology, please use \"-help\" to check for supported onces" << std::endl;
	  exit(1);
	}
	std::cout << "Using topology " << GlobalParams::topology << std::endl;
      }
      else if (!strcmp(argv[i], "-dimX"))
	GlobalParams::mesh_dim_x = atoi(argv[++i]);
      else if (!strcmp(argv[i], "-dimY"))
	GlobalParams::mesh_dim_y = atoi(argv[++i]);
      else if (!strcmp(argv[i], "-p_dimX"))
	GlobalParams::proc_arr_dim_x = atoi(argv[++i]);
      else if (!strcmp(argv[i], "-p_dimY"))
	GlobalParams::proc_arr_dim_y = atoi(argv[++i]);
      else if (!strcmp(argv[i], "-npn"))
	GlobalParams::neurons_per_node = atoi(argv[++i]);
      else if (!strcmp(argv[i], "-r2r_ll"))
	GlobalParams::r2r_link_length = atof(argv[++i]);
      else if (!strcmp(argv[i], "-evt_w"))
	GlobalParams::evt_width = atoi(argv[++i]);
      else if (!strcmp(argv[i], "-buffer"))
	GlobalParams::buffer_depth = atoi(argv[++i]);
      else if (!strcmp(argv[i], "-threshold"))
	GlobalParams::dl_threshold = atoi(argv[++i]);
      else if (!strcmp(argv[i], "-period"))
	GlobalParams::clock_period_ps = atoi(argv[++i]);
      else if (!strcmp(argv[i], "-reset"))
	GlobalParams::reset_time = atoi(argv[++i]);
      else if (!strcmp(argv[i], "-warmup"))
	GlobalParams::stats_warm_up_time = atoi(argv[++i]);
      else if (!strcmp(argv[i], "-sim"))
	GlobalParams::simulation_time = atoi(argv[++i]);
      else if (!strcmp(argv[i], "-rnd_seed"))
	GlobalParams::rnd_generator_seed = atoi(argv[++i]);
      else if (!strcmp(argv[i], "-rt_file"))
	GlobalParams::routing_table_filename = argv[++i];
      else if (!strcmp(argv[i], "-nest_t_mul"))
	GlobalParams::nest_time_multiplier = atof(argv[++i]);
      else if (!strcmp(argv[i], "-nest_presim"))
	GlobalParams::nest_t_presim = atof(argv[++i]);
      else if (!strcmp(argv[i], "-nest_skip"))
	GlobalParams::nest_spk_det_skip_lines = atoi(argv[++i]);
      else if (!strcmp(argv[i], "-gnat"))
	GlobalParams::gnat_method = argv[++i];
      else if (!strcmp(argv[i], "-gnat_str"))
	GlobalParams::gnat_string = argv[++i];
      else if (!strcmp(argv[i], "-verbose"))
	GlobalParams::verbose_mode = argv[++i];
      else if (!strcmp(argv[i], "-of_suffix"))
      {
	GlobalParams::output_file_suffix = argv[++i];
	GlobalParams::create_output_files = true;
      }
      else if (!strcmp(argv[i], "-buff_stat"))
	GlobalParams::show_buffer_stats = true;
      else if (!strcmp(argv[i], "-detailed"))
	GlobalParams::detailed = true;
      else if (!strcmp(argv[i], "-help"))
      {
	showHelp(argv[0]);
	exit(0);
      }
      else if (!strcmp(argv[i], "-config"))
	++i;  // Just skip - config is managed by configure(...)
      else if (!strcmp(argv[i], "-pwr_conf"))
	++i;  // Just skip - pwr_conf is managed by configure(...)
      else
      {
	std::cerr << "ERROR: Invalid option: " << argv[i] << std::endl;
	exit(1);
      }
    }
  }
}

void configure(int argc, char* argv[])
{
  bool config_found = false;
  bool power_config_found = false;

  for (int i = 1; i < argc; ++i)
  {
    if (!strcmp(argv[i], "-help"))
    {
      showHelp(argv[0]);
      exit(0);
    }
  }
  for (int i = 1; i < argc; ++i)
  {
    if (!strcmp(argv[i], "-config"))
    {
      GlobalParams::config_filename = argv[++i];
      config_found = true;
      break;
    }
  }
  if (!config_found)
  {
    std::ifstream infile(CONFIG_FILENAME);
    if (infile.good())
      GlobalParams::config_filename = CONFIG_FILENAME;
    else
    {
      std::cerr << "No YAML configuration file found!" << std::endl;
      std::cerr << "Use -config to load a file from a designated place" << std::endl;
      exit(0);
    }
  }
  for (int i = 1; i < argc; ++i)
  {
    if (!strcmp(argv[i], "-pwr_conf"))
    {
      GlobalParams::power_config_filename = argv[++i];
      power_config_found = true;
      break;
    }
  }
  if (!power_config_found)
  {
    std::ifstream infile(POWER_CONFIG_FILENAME);
    if (infile.good())
      GlobalParams::power_config_filename = POWER_CONFIG_FILENAME;
    else
    {
      std::cerr << "No YAML power configuration file found!" << std::endl;
      std::cerr << "Use -pwr_conf to load a file from a designated place" << std::endl;
    }
  }

  loadConfiguration();
  parseCmdLine(argc, argv);
  checkConfiguration();

  // Show configuration
  if (GlobalParams::verbose_mode == "VERBOSE_HIGH")
    showConfig();
}

template <typename T>
T readParam(YAML::Node node, std::string param, T default_value)
{
  try
  {
    return node[param].as<T>();
  }
  catch (std::exception &e)
  {
    return default_value;
  }
}

template <typename T>
T readParam(YAML::Node node, std::string param)
{
  try
  {
    return node[param].as<T>();
  }
  catch (std::exception &e)
  {
    std::cerr << "ERROR: Cannot read param " << param << ". " << std::endl;
    exit(0);
  }
}
