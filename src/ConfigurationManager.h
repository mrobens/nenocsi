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
 * 2022-09-11: Template specializations related to the wireless network are
 *             removed
 *             Template specialization for RouterPowerConfig adapted to the
 *             new router design
 *
 *///******************************************************************************** 

#ifndef __MCAERSIMCONFIGURATIONMANAGER_H__
#define __MCAERSIMCONFIGURATIONMANAGER_H__

#include "yaml-cpp/yaml.h"
#include "GlobalParams.h"

#include <iostream>
#include <string>
#include <cstring>
#include <fstream>
#include <vector>

void configure(int argc, char* argv[]);

template <typename T>
T readParam(YAML::Node node, std::string param, T default_value);

template <typename T>
T readParam(YAML::Node node, std::string param);

namespace YAML
{
  template<>
  struct convert<BufferPowerConfig>
  {
    static bool decode(const Node& node, BufferPowerConfig& bufferPowerConfig)
    {
      for(const_iterator buffering_it = node.begin(); buffering_it != node.end(); ++buffering_it)
      {
	std::vector<double> v = buffering_it->as<std::vector<double> >();
	bufferPowerConfig.leakage[std::make_pair(v[0], v[1])] = v[2];
	bufferPowerConfig.push[std::make_pair(v[0],v[1])] = v[3];
	bufferPowerConfig.front[std::make_pair(v[0], v[1])] = v[4];
	bufferPowerConfig.pop[std::make_pair(v[0], v[1])] = v[5];
      }
      return true;
    }
  };
  
  template<>
  struct convert<LinkBitLinePowerConfig>
  {
    static bool decode(const Node& node, LinkBitLinePowerConfig& linkBitLinePowerConfig)
    {
      for (const_iterator lbl_pc_it = node.begin(); lbl_pc_it != node.end(); ++lbl_pc_it)
      {
	std::vector<double> v = lbl_pc_it->as<std::vector<double> >();
	linkBitLinePowerConfig[v[0]] = std::make_pair(v[1], v[2]);
      }
      return true;
    }
  };

  template<>
  struct convert<RouterPowerConfig>
  {
    static bool decode(const Node& node, RouterPowerConfig& routerPowerConfig)
    {
      for (const_iterator xbar_it = node["crossbar"].begin(); xbar_it != node["crossbar"].end(); ++xbar_it)
      {
	std::vector<double> v = xbar_it->as<std::vector<double> >();
	routerPowerConfig.crossbar_pm[boost::tuples::make_tuple(v[0], v[1], v[2])] = std::make_pair(v[3], v[4]);
      }
      for (const_iterator ni_it = node["network_interface"].begin(); ni_it != node["network_interface"].end(); ++ni_it)
      {
	std::vector<double> v = ni_it->as<std::vector<double> >();
	routerPowerConfig.network_interface[v[0]] = std::make_pair(v[1], v[2]);
      }
      for (const_iterator arb_it = node["arbitration"].begin(); arb_it != node["arbitration"].end(); ++arb_it)
      {
	std::vector<double> v = arb_it->as<std::vector<double> >();
	routerPowerConfig.arbitration_pm[v[0]] = std::make_pair(v[1], v[2]);
      }
      for (const_iterator tcam_it = node["tcam_access"].begin(); tcam_it != node["tcam_access"].end(); ++tcam_it)
      {
	std::vector<double> v = tcam_it->as<std::vector<double> >();
	routerPowerConfig.tcam_pm[v[0]] = std::make_pair(v[1], v[2]);
      }
      for (const_iterator sram_it = node["sram_access"].begin(); sram_it != node["sram_access"].end(); ++sram_it)
      {
	std::vector<double> v = sram_it->as<std::vector<double> >();
	routerPowerConfig.sram_pm[v[0]] = std::make_pair(v[1], v[2]);
      }
      return true;
    }
  };

  template<>
  struct convert<PowerConfig>
  {
    static bool decode(const Node& node, PowerConfig& powerConfig)
    {
      powerConfig.bufferPowerConfig = node["Buffer"].as<BufferPowerConfig>();
      powerConfig.linkBitLinePowerConfig = node["LinkBitLine"].as<LinkBitLinePowerConfig>();
      powerConfig.routerPowerConfig = node["Router"].as<RouterPowerConfig>();
      return true;
    }
  };
}

#endif /* __MCAERSIMCONFIGURATIONMANAGER_H__ */
