/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2018 by the University of Catania
 * For the complete list of authors refer to file ../../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../../doc/LICENSE.txt
 *
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
 * refer to file ../../doc/LICENSE_MCAERSIM.txt
 * 
 * 2022-09-07: This file contains the implementation of the global neuron assignment
 *             table map. It has been implemented in parallel to RoutingAlgorithms.cpp
 *             of the Noxim release, but provides for plugging-in classes with
 *             methods to retrieve the parameters needed to fill the local event
 *             queues rather than classes with different routing methods. 
 *
 *///******************************************************************************** 

#include "GlobalNeuronAssignmentTables.h"

GlobalNeuronAssignmentTablesMap* GlobalNeuronAssignmentTables::globalNeuronAssignmentTablesMap = 0;

GlobalNeuronAssignmentTable* GlobalNeuronAssignmentTables::get(const std::string& globalNeuronAssignmentTableName)
{
  GlobalNeuronAssignmentTablesMap::iterator it = getGlobalNeuronAssignmentTablesMap()->find(globalNeuronAssignmentTableName);
  if (it == getGlobalNeuronAssignmentTablesMap()->end())
    return 0;

  return it->second;
}

GlobalNeuronAssignmentTablesMap* GlobalNeuronAssignmentTables::getGlobalNeuronAssignmentTablesMap()
{
  if (globalNeuronAssignmentTablesMap == 0)
    globalNeuronAssignmentTablesMap = new GlobalNeuronAssignmentTablesMap();
  return globalNeuronAssignmentTablesMap;
}
