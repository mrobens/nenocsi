/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2018 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the implementation of the global routing table
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
 * 2022-08-24: Like stated in GlobalRoutingTable.h, the routing table differs
 *             from the routing table used in Noxim. The load(...)-method
 *             therefore differs from the corresponding implementation in
 *             Noxim as well. In particular, data are read-in via YAML
 *             Nodes rather than by an input file stream in a similar fashion
 *             as configuration files are loaded in ConfigurationManager.cpp
 *             of Noxim.
 *
 *///******************************************************************************** 

#include "GlobalRoutingTable.h"
#include "yaml-cpp/yaml.h"

GlobalRoutingTable::GlobalRoutingTable()
{
  valid = false;
}

bool GlobalRoutingTable::load(const std::string& fname)
{
  // Load YAML document containing the global routing table
  YAML::Node gRT;
  try {
    gRT = YAML::LoadFile( fname );
    std::cout << "Loaded global routing table." << std::endl;
  }
  catch (YAML::BadFile &e) {
    std::cout << "Failed loading global routing table." << std::endl;
    std::cerr << "The specified YAML file was not found!" << std::endl;
    return false;
  }
  catch (YAML::ParserException &pe) {
    std::cout << "Failed loading global routing table." << std::endl;
    std::cerr << "ERROR at line " << pe.mark.line + 1 << " column " << pe.mark.column + 1;
    std::cerr << ": " << pe.msg << ". Please check indentation." << std::endl;
    return false;
  }
  global_rt.clear();
  for (YAML::const_iterator itNodes = gRT.begin(); itNodes != gRT.end(); ++itNodes) {
    LocalRT tempRTlocal;
    for (YAML::const_iterator itRTlocal = itNodes->second.begin(); itRTlocal != itNodes->second.end(); ++itRTlocal) {
      outDirs tempOut;
      for (YAML::const_iterator itOutputs = itRTlocal->second.begin(); itOutputs != itRTlocal->second.end(); ++itOutputs) {
	tempOut.insert((*itOutputs).as<int>());
      }
      tempRTlocal[itRTlocal->first.as<int>()] = tempOut;
    }
    global_rt[itNodes->first.as<int>()] = tempRTlocal;
  }
  valid = true;
  return true;
}

LocalRT GlobalRoutingTable::getLocalRoutingTable(const int node_id)
{
  return global_rt[node_id];
}
