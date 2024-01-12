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
 * 2022-09-07: This file contains the declaration of the global neuron assignment
 *             table map. It has been implemented in parallel to RoutingAlgorithms.h
 *             of the Noxim release, but provides for plugging-in classes with
 *             methods to retrieve the parameters needed to fill the local event
 *             queues rather than classes with different routing methods. 
 *
 *///******************************************************************************** 

#ifndef __MCAERSIMGLOBALNEURONASSIGNMENTTABLES_H__
#define __MCAERSIMGLOBALNEURONASSIGNMENTTABLES_H__

#include <map>
#include <string>
#include "GlobalNeuronAssignmentTable.h"

typedef std::map<std::string, GlobalNeuronAssignmentTable* > GlobalNeuronAssignmentTablesMap;

class GlobalNeuronAssignmentTables
{
 public:
  static GlobalNeuronAssignmentTablesMap* globalNeuronAssignmentTablesMap;
  static GlobalNeuronAssignmentTablesMap* getGlobalNeuronAssignmentTablesMap();

  static GlobalNeuronAssignmentTable* get(const std::string& globalNeuronAssignmentTableName);
};

struct GlobalNeuronAssignmentTablesRegister: GlobalNeuronAssignmentTables
{
  GlobalNeuronAssignmentTablesRegister(const std::string& globalNeuronAssignmentTableName, GlobalNeuronAssignmentTable* globalNeuronAssignmentTable)
  {
    getGlobalNeuronAssignmentTablesMap()->insert(std::make_pair(globalNeuronAssignmentTableName, globalNeuronAssignmentTable));
  }
};

#endif /* __MCAERSIMGLOBALNEURONASSIGNMENTTABLES_H__ */
