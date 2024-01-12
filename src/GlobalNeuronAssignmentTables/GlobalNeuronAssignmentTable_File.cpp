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
 *             table. This table is an auxiliary data structure that helps to
 *             configure the processing elements or more specifically their local
 *             event queues based on a file. This file contains two nested
 *             dictionaries. The keys of the top-level dictionary contain the
 *             processing element IDs, the keys of the nested dictionaries contain
 *             the names of the associated spike recorder files. The value of the
 *             nested dictionary is a list of the first and the last neuron that
 *             are served by the processing element.
 *             Code segments required for the plug-in mechanism are in line with
 *             respective code segments in, e.g. Routing_XY.cpp of the Noxim release. 
 *             However, there are some additions. Especially, virtual prototype 
 *             methods of the abstract base class GlobalNeuronAssignmentTable are 
 *             overloaded in such a way that local event queues of the processing 
 *             elements are populated consistent with a custom neuron mapping. 
 *             (I.e., neuron populations are assigned to available processing 
 *             elements according to the information provided in the custom
 *             neuron assignment configuration file adhering to the format 
 *             described above.)
 *             The configuration file is read-in by the load(...) method in a 
 *             similar way as the configuration file conf.yaml in 
 *             ConfigurationManager.cpp of the Noxim release.
 *
 *///******************************************************************************** 

#include "GlobalNeuronAssignmentTable_File.h"

GlobalNeuronAssignmentTablesRegister GlobalNeuronAssignmentTable_File::globalNeuronAssignmentTablesRegister("gnat_file", getInstance());

GlobalNeuronAssignmentTable_File* GlobalNeuronAssignmentTable_File::globalNeuronAssignmentTable_File = 0;

GlobalNeuronAssignmentTable_File* GlobalNeuronAssignmentTable_File::getInstance()
{
  if (globalNeuronAssignmentTable_File == 0)
    globalNeuronAssignmentTable_File = new GlobalNeuronAssignmentTable_File();

  return globalNeuronAssignmentTable_File;
}

bool GlobalNeuronAssignmentTable_File::load(const std::string& pe_conf_input_filename)
{
  YAML::Node gnat_yaml;
  try
  {
    gnat_yaml = YAML::LoadFile( pe_conf_input_filename );
    std::cout << "Loaded global neuron assignment table." << std::endl;
  }
  catch (YAML::BadFile &e)
  {
    std::cerr << "Failed loading global neuron assignment table." << std::endl;
    std::cerr << "The specified YAML file was not found!" << std::endl;
    return false;
  }
  catch (YAML::ParserException &pe)
  {
    std::cerr << "Failed loading global neuron assignment table." << std::endl;
    std::cerr << "ERROR at line " << pe.mark.line + 1 << " column ";
    std::cerr << pe.mark.column + 1 << ": " << pe.msg;
    std::cerr << ". Please check indentation." << std::endl;
    return false;
  }
  peConfigMap.clear();
  peConfigMap = gnat_yaml.as<PEconfigMap>();
  valid = true;
  return true;
}

bool GlobalNeuronAssignmentTable_File::getSpkRecFileParams(const int node_id, const int proc_id, std::string& spk_rec_fname, int& id_min, int& id_max)
{
  if (!isValid())
  {
    std::cerr << "GlobalNeuronAssignmentTable was not successfully initialized before!" << std::endl;
    return false;
  }
  int proc_abs = NO_PES * node_id + proc_id;
  if (peConfigMap.find(proc_abs) == peConfigMap.end())
  {
    std::cerr << "ERROR: Configuration for PE " << proc_abs << "could not be found in YAML file." << std::endl;
    return false;
  }
  PEconfig pec = peConfigMap[proc_abs];
  spk_rec_fname = pec.spike_recorder;
  id_min = pec.first_neuron;
  id_max = pec.second_neuron;
  return true;
}
    
