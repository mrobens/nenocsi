/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2018 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
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
 * 2022-10-19: This file contains the implementation of rtparser used to convert 
 *             a connectivity file created by NEST to a routing table. Principally,
 *             it was written from scratch. However, it uses code segments from
 *             GlobalRoutingTable.cpp, ConfigurationManager.cpp, DataStructs.h, and
 *             Utils.h of the Noxim release. 
 *
 *///******************************************************************************** 

#include "GlobalParamsParser.h"
#include "yaml-cpp/yaml.h"
#include <boost/filesystem.hpp>
#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <fstream>
#include <cmath>
#include <cstring>

enum
{
 DIRECTION_NORTH,
 DIRECTION_EAST,
 DIRECTION_SOUTH,
 DIRECTION_WEST,
 DIRECTIONS
};

typedef std::vector<std::pair<int, int> > Pop_Limits;
typedef std::map<int, std::set<int> > LocalRT;
typedef std::map<int, LocalRT> GlobalRT;

bool loadPopIDs(const std::string fname, Pop_Limits& pop_ids)
{
  std::ifstream fin(fname, std::ios::in);
  if (!fin)
    return false;

  bool stop = false;
  bool valid = false;
  while (!fin.eof() && !stop)
  {
    char line[40];
    fin.getline(line, sizeof(line) - 1);
    if (line[0] == '\0')
      stop = true;
    else
    {
      int n1, n2;
      if (sscanf(line, "%d %d", &n1, &n2) == 2)
      {
	pop_ids.push_back(std::make_pair(n1, n2));
      }
      else
      {
	std::cerr << "ERROR: File contains malformed line" << std::endl;
	exit(1);
      }
    }
  }
  if (pop_ids.size() > 0)
    valid = true;
  return valid;
}

void determineAccumulatedPEs(const Pop_Limits& pop_ids, std::vector<int>& accumulatedPEs)
{
  for (Pop_Limits::const_iterator it = pop_ids.begin(); it != pop_ids.end(); ++it)
  {
    int pop_PEs = (int)ceil((double)(it->second - it->first + 1) / (double) GlobalParams::nrs_p_node);
    if (accumulatedPEs.size() > 0)
      accumulatedPEs.push_back(accumulatedPEs.back() + pop_PEs);
    else
      accumulatedPEs.push_back(pop_PEs);
  }
}

bool loadYAML(const std::string fname, YAML::Node& node)
{
  try
  {
    node = YAML::LoadFile(fname);
    std::cout << "Loaded YAML file " << fname << "." << std::endl;
  }
  catch (YAML::BadFile &e)
  {
    std::cerr << "ERROR: Could not load file " << fname << "." <<  std::endl;
    std::cerr << "The specified YAML file was not found!" << std::endl;
    exit(1);
  }
  catch (YAML::ParserException &pe)
  {
    std::cerr << "Failed loading file " << fname << "." << std::endl;
    std::cerr << "ERROR at line " << pe.mark.line + 1 << " column " << pe.mark.column + 1 << ": " << pe.msg << "." << std::endl;
    std::cerr << "Please check indentation." << std::endl;
    exit(1);
  }
  return true;
}

bool parseParams()
{
  YAML::Node params;
  if (loadYAML("parser_config.yaml", params))
  {
    try
    {
      GlobalParams::mesh_dim_x = params["mesh_dim_x"].as<int>();
      GlobalParams::mesh_dim_y = params["mesh_dim_y"].as<int>();
      GlobalParams::number_pes = params["number_pes"].as<int>();
      GlobalParams::nrs_p_node = params["nrs_p_node"].as<int>();
      GlobalParams::topology = params["topology"].as<std::string>();
      GlobalParams::output_file_name = params["output_file_name"].as<std::string>();
    }
    catch (std::exception &e)
    {
      std::cerr << "ERROR: Could not populate the global parameters successfully!" << std::endl;
      exit(1);
    }
  }
  else
  {
    std::cerr << "ERROR: Could not load and parse the global parameters successfully!" << std::endl;
    exit(1);
  }
  return true;
}

struct Coord
{
  int x;
  int y;
  Coord()
  {
    x = 0;
    y = 0;
  }
  Coord(int cx, int cy)
  {
    x = cx;
    y = cy;
  }
  inline bool operator==(const Coord& coord)
  {
    return ((coord.x == x) && (coord.y == y));
  }
};

inline Coord id2coord(const int tile_id)
{
  Coord coord;
  coord.x = tile_id % GlobalParams::mesh_dim_x;
  coord.y = tile_id / GlobalParams::mesh_dim_x;
  if ((coord.x >= GlobalParams::mesh_dim_x) || (coord.y >= GlobalParams::mesh_dim_y))
  {
    std::cerr << "Coord out of range." << std::endl;
    exit(1);
  }
  return coord;
}

inline int coord2id(const Coord coord)
{
  int tile_id = (coord.y * GlobalParams::mesh_dim_x) + coord.x;
  if (!(tile_id < GlobalParams::mesh_dim_x * GlobalParams::mesh_dim_y))
  {
    std::cerr << "Tile ID out of range." << std::endl;
    exit(1);
  }
  return tile_id;
}
  

void getCoordAndPE(const int nrn_gid, const Pop_Limits& pop_lim, const std::vector<int>& accPEs, Coord& coord, int& locPE)
{
  int pop_id = 0;                                                                   // ID of the population
  int nrn_lid = 0;                                                                  // Local neuron ID within the population (starts with 1)
  int lPE = 0;                                                                      // Local processor element ID w.r.t. the populations
  int gPE = 0;                                                                      // Global processor element ID w.r.t. the populations
  int tile_id = 0;                                                                  // Tile ID
  
  for (size_t i = 0; i < pop_lim.size(); ++i)
  {
    if ((nrn_gid >= pop_lim[i].first) && (nrn_gid <= pop_lim[i].second))
      pop_id = i;
  }
  if (pop_id > 0)
    nrn_lid = nrn_gid - pop_lim[pop_id - 1].second;
  else
    nrn_lid = nrn_gid;
  
  lPE = (int)ceil((double)nrn_lid / (double)GlobalParams::nrs_p_node);              // Results in 1-based PE IDs
  if (pop_id > 0)                                                                   // Results in 1-based gPE IDs
    gPE = lPE + accPEs[pop_id - 1];
  else
    gPE = lPE;
  
  tile_id = (int)(gPE - 1) / GlobalParams::number_pes;                              // Tile ID 0-based
  locPE = (int)(gPE - 1) % GlobalParams::number_pes;                                // Locale PE w.r.t. the PEs 0-based
  coord = id2coord(tile_id);                                                        // Coord within the mesh of tiles
}
  
    

int main(int argc, char* argv[])
{
  std::cout << std::endl;
  std::cout << "    ------------------------------------------------------------------------" << std::endl;
  std::cout << "     Noxim - the NoC Simulator " << std::endl;
  std::cout << "     Copyright (C) 2005-2018 by the University of Catania" << std::endl;
  std::cout << "    ------------------------------------------------------------------------" << std::endl;
  std::cout << "     RTparser - Routing Table Parser for McAERsim" << std::endl;
  std::cout << "     Modifications Copyright 2022-2023 Forschungszentrum Juelich GmbH, ZEA-2" << std::endl;
  std::cout << std::endl;
  std::cout << "     This program is provided AS IS and comes with ABSOLUTELY NO WARRANTY." << std::endl;
  std::cout << "     It is free software, and you are welcome to redistribute it under" << std::endl;
  std::cout << "     certain conditions. See the LICENSE_MCAERSIM.txt file for details." << std::endl;
  std::cout << "    ------------------------------------------------------------------------" << std::endl;
  std::cout << std::endl;
  if (argc < 3)
  {
    std::cout << "Usage: " << argv[0] << " pop_id_file neuron_conn_file" << std::endl
              << "Where pop_id_file and neuron_conn_file identify the following:" << std::endl
              << std::endl
              << "pop_id_file\t\tThe file containing the population limits, i.e. the " << std::endl
              << "\t\t\tfirst and last neuron in each population." << std::endl
              << "neuron_conn_file\tThe file containing all the connections as YAML dictionary." << std::endl
              << "\t\t\tTop level keys identify respective processing elements." << std::endl
              << "\t\t\tThe key of a nested dictionary identifies the source neuron," << std::endl
              << "\t\t\tthe value list contains all the target neurons." << std::endl
              << std::endl;
    exit(1);
  }
  bool success = true;
  success = success && parseParams();
  // Determine two vectors, one containing pairs
  // of neuron ID numbers corresponding to a
  // population and one containing the
  // accumulated number of PEs that is required
  // to store a population as well as the previous
  // once.
  Pop_Limits popIDs;
  std::vector<int> accPEs;
  loadPopIDs(std::string(argv[1]), popIDs);
  determineAccumulatedPEs(popIDs, accPEs);

  // Load the neuron connections that should be parsed into the routing table
  GlobalRT grt;
  YAML::Node conns;
  success = success && loadYAML(std::string(argv[2]), conns);
 
  // Parse the connection information into a routing table using XY routing
  try
  {
    for (YAML::const_iterator pop_it = conns.begin(); pop_it != conns.end(); ++pop_it)
      for (YAML::const_iterator src_it = pop_it->second.begin(); src_it != pop_it->second.end(); ++src_it)
        for (YAML::const_iterator dst_it = src_it->second.begin(); dst_it != src_it->second.end(); ++dst_it)
	{
	  // Source neuron and its coordinates
	  int nrn_src = src_it->first.as<int>();
	  Coord coord_src;
	  int lpe_src;
	  getCoordAndPE(nrn_src, popIDs, accPEs, coord_src, lpe_src);
	  // Destination neuron and its coordinates
	  int nrn_dst = dst_it->as<int>();
	  Coord coord_dst;
	  int lpe_dst;
	  getCoordAndPE(nrn_dst, popIDs, accPEs, coord_dst, lpe_dst);
	  //if ((coord_src == coord_dst) && (lpe_src == lpe_dst))             // Modified 2022/09/23: Processing elements will inject source
	  //{                                                                 // neuron IDs not knowing, whether they are intended for pure
	  //  continue; // AER_EVT information kept in local PE               // local delivery or not. If there is no entry in the routing
	  //}                                                                 // table in such a case, CAM look-up would fail and the AER_EVT
	  //else if (coord_src == coord_dst)                                  // would be trapped.
	  if (coord_src == coord_dst)
	  {
	    grt[coord2id(coord_src)][nrn_src].insert(DIRECTIONS + lpe_dst);
	  }
	  else
	  {
	    if (GlobalParams::topology.compare("TOPOLOGY_MESH") == 0)
	    {
	      int hops_x = coord_dst.x - coord_src.x;
	      int hops_y = coord_dst.y - coord_src.y;
	      int x_src = coord_src.x;
	      int y_src = coord_src.y;
	      int hx = 0;
	      int hy = 0;
	      if (hops_x > 0)                                                                                      // x direction first
	      {
	        for (hx = 0; hx < hops_x; hx++)
	        {
		  grt[coord2id(Coord(x_src + hx, y_src))][nrn_src].insert(DIRECTION_EAST);
	        }
	      }
	      else if (hops_x < 0)
	      {
	        for (hx = 0; hx > hops_x; hx--)
	        {
		  grt[coord2id(Coord(x_src + hx, y_src))][nrn_src].insert(DIRECTION_WEST);
	        }
	      }
	      if (hops_y > 0)                                                                                      // y direction second
	      {
	        for (hy = 0; hy < hops_y; hy++)
	        {
		  grt[coord2id(Coord(x_src + hx, y_src + hy))][nrn_src].insert(DIRECTION_SOUTH);
	        }
	      }
	      else if (hops_y < 0)
	      {
	        for (hy = 0; hy > hops_y; hy--)
	        {
		  grt[coord2id(Coord(x_src + hx, y_src + hy))][nrn_src].insert(DIRECTION_NORTH);
	        }
	      }
	      grt[coord2id(Coord(x_src + hops_x, y_src + hops_y))][nrn_src].insert(DIRECTIONS + lpe_dst);         // local PE last
	    }
	    else if (GlobalParams::topology.compare("TOPOLOGY_TORUS") == 0)
	    {
	      int hops_x = coord_dst.x - coord_src.x;
	      int hops_y = coord_dst.y - coord_src.y;
	      if (abs(hops_x) > (GlobalParams::mesh_dim_x / 2))
	      {
	        if (hops_x > 0)
	        {
	          int hops_x_e = GlobalParams::mesh_dim_x - hops_x;
	          for (int hx = 0; hx > -hops_x_e; hx--)
		  {
		    int x_rem = (coord_src.x + GlobalParams::mesh_dim_x + hx) % GlobalParams::mesh_dim_x;
		    int y_rem = coord_src.y;
		    grt[coord2id(Coord(x_rem, y_rem))][nrn_src].insert(DIRECTION_WEST);
		  }
	        }
	        else if (hops_x < 0)
	        {
		  int hops_x_e = -GlobalParams::mesh_dim_x - hops_x;
		  for (int hx = 0; hx < -hops_x_e; hx++)
		  {
		    int x_rem = (coord_src.x + hx) % GlobalParams::mesh_dim_x;
		    int y_rem = coord_src.y;
		    grt[coord2id(Coord(x_rem, y_rem))][nrn_src].insert(DIRECTION_EAST);
		  }
	        }
	      }
	      else
	      {
	        if (hops_x > 0)
	        {
		  for (int hx = 0; hx < hops_x; hx++)
		  {
		    int x_rem = coord_src.x + hx;
		    int y_rem = coord_src.y;
		    grt[coord2id(Coord(x_rem, y_rem))][nrn_src].insert(DIRECTION_EAST);
		  }
	        }
	        if (hops_x < 0)
	        {
		  for (int hx = 0; hx > hops_x; hx--)
		  {
		    int x_rem = coord_src.x + hx;
		    int y_rem = coord_src.y;
		    grt[coord2id(Coord(x_rem, y_rem))][nrn_src].insert(DIRECTION_WEST);
		  }
	        }	  
	      }
	      if (abs(hops_y) > (GlobalParams::mesh_dim_y / 2))
	      {
	        if (hops_y > 0)
	        {
		  int hops_y_e = GlobalParams::mesh_dim_y - hops_y;
		  for (int hy = 0; hy > -hops_y_e; hy--)
		  {
		    int x_rem = coord_src.x + hops_x;
		    int y_rem = (coord_src.y + GlobalParams::mesh_dim_y + hy) % GlobalParams::mesh_dim_y;
		    grt[coord2id(Coord(x_rem, y_rem))][nrn_src].insert(DIRECTION_NORTH);
		  }
	        }
	        else if (hops_y < 0)
	        {
		  int hops_y_e = -GlobalParams::mesh_dim_y - hops_y;
		  for (int hy = 0; hy < -hops_y_e; hy++)
		  {
		    int x_rem = coord_src.x + hops_x;
		    int y_rem = (coord_src.y + hy) % GlobalParams::mesh_dim_y;
		    grt[coord2id(Coord(x_rem, y_rem))][nrn_src].insert(DIRECTION_SOUTH);
		  }
	        }
	      }
	      else
	      {
	        if (hops_y > 0)
	        {
		  for (int hy = 0; hy < hops_y; hy++)
		  {
		    int x_rem = coord_src.x + hops_x;
		    int y_rem = coord_src.y + hy;
		    grt[coord2id(Coord(x_rem, y_rem))][nrn_src].insert(DIRECTION_SOUTH);
		  }
	        }
	        else if (hops_y < 0)
	        {
		  for (int hy = 0; hy > hops_y; hy--)
		  {
		    int x_rem = coord_src.x + hops_x;
		    int y_rem = coord_src.y + hy;
		    grt[coord2id(Coord(x_rem, y_rem))][nrn_src].insert(DIRECTION_NORTH);
		  }
	        }	  
	      }
	      grt[coord2id(Coord(coord_src.x + hops_x, coord_src.y + hops_y))][nrn_src].insert(DIRECTIONS + lpe_dst);
	    }
	    else
	    {
	      std::cout << "Unknown topology. Please select TOPOLOGY_MESH or TOPOLOGY_TORUS in the configuration file!" << std::endl;
	      exit(1);
	    }
	  }
	}
  }
  catch (std::exception &e)
  {
    std::cerr << "ERROR: Could not convert connection data to global routing table successfully." << std::endl;
    exit(1);
  }
  YAML::Emitter out;
  out.SetSeqFormat(YAML::Flow);
  out << YAML::BeginMap;
  for (GlobalRT::const_iterator git = grt.begin(); git != grt.end(); ++git)
  {
    out << YAML::Key;
    out << git->first;
    out << YAML::Value;
    out << YAML::BeginMap;
    for (LocalRT::const_iterator lit = git->second.begin(); lit != git->second.end(); ++lit)
    {
      out << YAML::Key;
      out << lit->first;
      out << YAML::Value;
      out << lit->second;
    }
    out << YAML::EndMap;
  }
  out << YAML::EndMap;
  boost::filesystem::path data("../nest_inputs/data");
  try
  {
    bool created = boost::filesystem::create_directory(data);
    if (created) std::cout << "Created folder " << data.c_str() << " used for YAML output file." << std::endl;
    else std::cout << "Folder " << data.c_str() << " already existed. Using it for output file." << std::endl;
  }
  catch (const boost::filesystem::filesystem_error &fse)
  {
    std::cerr << fse.what() << std::endl;
    exit(1);
  }
  std::ofstream of_dout((data /= GlobalParams::output_file_name).c_str());
  if (!of_dout)
    exit(1);
  of_dout << out.c_str() << std::endl;
  of_dout.close();
  return 0;
}
