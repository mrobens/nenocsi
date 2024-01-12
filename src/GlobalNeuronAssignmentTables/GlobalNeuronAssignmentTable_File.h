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
 *             table. This table is an auxiliary data structure that helps to
 *             configure the processing elements or more specifically their local
 *             event queues based on a file. This file contains two nested
 *             dictionaries. The keys of the top-level dictionary contain the
 *             processing element IDs, the keys of the nested dictionaries contain
 *             the names of the associated spike recorder files. The value of the
 *             nested dictionary is a list of the first and the last neuron that
 *             are served by the processing element.
 *             Code segments required for the plug-in mechanism are in line with
 *             respective code segments in, e.g. Routing_XY.h of the Noxim release. 
 *             However, there are several additions. Especially, virtual prototype 
 *             methods of the abstract base class GlobalNeuronAssignmentTable are 
 *             overloaded in such a way that local event queues of the processing 
 *             elements are populated consistent with a custom neuron mapping. 
 *             (I.e., neuron populations are assigned to available processing 
 *             elements according to the information provided in the custom
 *             neuron assignment configuration file adhering to the format 
 *             described above.)
 *
 *///******************************************************************************** 

#ifndef __MCAERSIMGLOBALNEURONASSIGNMENTTABLE_FILE_H__
#define __MCAERSIMGLOBALNEURONASSIGNMENTTABLE_FILE_H__

#include <iostream>
#include "yaml-cpp/yaml.h"
#include "GlobalParams.h"
#include "DataStructs.h"
#include "GlobalNeuronAssignmentTable.h"
#include "GlobalNeuronAssignmentTables.h"

struct GlobalNeuronAssignmentTable_File: GlobalNeuronAssignmentTable
{
  // Constructor and Destructor
  GlobalNeuronAssignmentTable_File(){};
  ~GlobalNeuronAssignmentTable_File(){};
  // Methods
  bool load(const std::string& pe_conf_input_filename);
  bool getSpkRecFileParams(const int node_id, const int proc_id, std::string& spk_rec_fname, int& id_min, int& id_max);
  bool isValid() { return valid; }
  static GlobalNeuronAssignmentTable_File* getInstance();
 private:
  bool valid;
  PEconfigMap peConfigMap;
  static GlobalNeuronAssignmentTable_File* globalNeuronAssignmentTable_File;
  static GlobalNeuronAssignmentTablesRegister globalNeuronAssignmentTablesRegister;
};

namespace YAML
{
  template<>
  struct convert<PEconfigMap>
  {
    static bool decode(const Node& node, PEconfigMap& peConfigMap)
    {
      for (const_iterator map_it = node.begin(); map_it != node.end(); ++map_it)
      {
	PEconfig pec;
	const_iterator conf_it = map_it->second.begin();
	pec.spike_recorder = conf_it->first.as<std::string>();
	std::vector<int> limits = conf_it->second.as<std::vector<int> >();
	pec.first_neuron = limits[0];
	pec.second_neuron = limits[1];
	peConfigMap[map_it->first.as<int>()] = pec;
      }
      return true;
    }
  };
}

#endif /* __MCAERSIMGLOBALNEURONASSIGNMENTTABLE_FILE_H__ */
