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
 * For the license applied to these modifications an McAERsim as a whole
 * refer to file ../doc/LICENSE_MCAERSIM.txt
 * 
 * 2022-09-14: This file initializes the few retained parameters needed by rtparser 
 *             to convert a connectivity file created by NEST to a routing table. 
 *
 *///******************************************************************************** 

#include "GlobalParamsParser.h"

int GlobalParams::mesh_dim_x;
int GlobalParams::mesh_dim_y;
int GlobalParams::number_pes;
int GlobalParams::nrs_p_node;
std::string GlobalParams::topology;
std::string GlobalParams::output_file_name;
