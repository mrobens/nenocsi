/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2018 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the implementation of the Network-on-Chip
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
 * 2022-09-11: This file has been largely simplified compared to its counterpart 
 *             in Noxim, since only methods buildMesh() and searchNode(...) remain 
 *             under the restrictions stated in NoC.h. As explained in NoC.h, 
 *             buildCommon() is no longer required within buildMesh(), since the few 
 *             remaining tasks are performed in the constructor. Method buildMesh() 
 *             has been further simplified, since wireless networks and advanced 
 *             selection strategies are not suported. On the other hand, support 
 *             for multiple processing elements per tile has been added as well as 
 *             the initialization of the individual local event queues based on 
 *             information provided by the global neuron assignment table. Also, 
 *             method buildTorus() has been added, which is similar to buildMesh(). 
 *             It just doesn't inactivate unused signals at the boundary nodes, but 
 *             rather provides wrap-around connections instead.
 *
 *///******************************************************************************** 

#include "NoC.h"

void NoC::buildMesh()
{
  // Initialize signals
  int dimX = GlobalParams::mesh_dim_x + 1;
  int dimY = GlobalParams::mesh_dim_y + 1;

  evt = new sc_signal_NESW<AER_EVT>*[dimX];
  req = new sc_signal_NESW<bool>*[dimX];
  ack = new sc_signal_NESW<bool>*[dimX];
  buffer_full_status = new sc_signal_NESW<bool>*[dimX];

  for (int i=0; i < dimX; ++i)
  {
    evt[i] = new sc_signal_NESW<AER_EVT>[dimY];
    req[i] = new sc_signal_NESW<bool>[dimY];
    ack[i] = new sc_signal_NESW<bool>[dimY];
    buffer_full_status[i] = new sc_signal_NESW<bool>[dimY];
  }

  t = new Tile**[GlobalParams::mesh_dim_x];
  for (int i=0; i < GlobalParams::mesh_dim_x; ++i)
  {
    t[i] = new Tile*[GlobalParams::mesh_dim_y];
  }

  // Create the mesh as a matrix of tiles
  for (int j = 0; j < GlobalParams::mesh_dim_y; ++j)
  {
    for (int i = 0; i < GlobalParams::mesh_dim_x; ++i)
    {
      // Create a single tile with a proper name
      char tile_name[64];
      Coord tile_coord;
      tile_coord.x = i;
      tile_coord.y = j;
      int tile_id = coord2Id(tile_coord);
      sprintf(tile_name, "Tile[%02d][%02d]_(#%d)", i, j, tile_id);
      t[i][j] = new Tile(tile_name, tile_id);

      // Configure router
      t[i][j]->r->configure(tile_id, GlobalParams::stats_warm_up_time,
			    GlobalParams::buffer_depth, grtable);
      t[i][j]->r->power.configureRouter(GlobalParams::evt_width, GlobalParams::buffer_depth,
					GlobalParams::evt_width);

      // Configure the processing element(s)
      for (int k=0; k < NO_PES; ++k)
      {
	t[i][j]->pe[k]->tile_id = tile_id;
	t[i][j]->pe[k]->local_id = k;	
	std::string spk_rec_fname;
	int min_n_id, max_n_id;
	if (gnat->getSpkRecFileParams(tile_id, k, spk_rec_fname, min_n_id, max_n_id))
	  t[i][j]->pe[k]->l_evt_q.init(spk_rec_fname, min_n_id, max_n_id);
	else
	  t[i][j]->pe[k]->l_evt_q.invalidate();
      }

      // Map clock and reset
      t[i][j]->clock(clock);
      t[i][j]->reset(reset);

      // Map RX signals
      t[i][j]->evt_rx[DIRECTION_NORTH](evt[i][j].south);
      t[i][j]->req_rx[DIRECTION_NORTH](req[i][j].south);
      t[i][j]->ack_rx[DIRECTION_NORTH](ack[i][j].north);
      t[i][j]->buffer_full_status_rx[DIRECTION_NORTH](buffer_full_status[i][j].north);

      t[i][j]->evt_rx[DIRECTION_EAST](evt[i + 1][j].west);
      t[i][j]->req_rx[DIRECTION_EAST](req[i + 1][j].west);
      t[i][j]->ack_rx[DIRECTION_EAST](ack[i + 1][j].east);
      t[i][j]->buffer_full_status_rx[DIRECTION_EAST](buffer_full_status[i + 1][j].east);

      t[i][j]->evt_rx[DIRECTION_SOUTH](evt[i][j + 1].north);
      t[i][j]->req_rx[DIRECTION_SOUTH](req[i][j + 1].north);
      t[i][j]->ack_rx[DIRECTION_SOUTH](ack[i][j + 1].south);
      t[i][j]->buffer_full_status_rx[DIRECTION_SOUTH](buffer_full_status[i][j + 1].south);

      t[i][j]->evt_rx[DIRECTION_WEST](evt[i][j].east);
      t[i][j]->req_rx[DIRECTION_WEST](req[i][j].east);
      t[i][j]->ack_rx[DIRECTION_WEST](ack[i][j].west);
      t[i][j]->buffer_full_status_rx[DIRECTION_WEST](buffer_full_status[i][j].west);

      //Map TX signals
      t[i][j]->evt_tx[DIRECTION_NORTH](evt[i][j].north);
      t[i][j]->req_tx[DIRECTION_NORTH](req[i][j].north);
      t[i][j]->ack_tx[DIRECTION_NORTH](ack[i][j].south);
      t[i][j]->buffer_full_status_tx[DIRECTION_NORTH](buffer_full_status[i][j].south);

      t[i][j]->evt_tx[DIRECTION_EAST](evt[i + 1][j].east);
      t[i][j]->req_tx[DIRECTION_EAST](req[i + 1][j].east);
      t[i][j]->ack_tx[DIRECTION_EAST](ack[i + 1][j].west);
      t[i][j]->buffer_full_status_tx[DIRECTION_EAST](buffer_full_status[i + 1][j].west);

      t[i][j]->evt_tx[DIRECTION_SOUTH](evt[i][j + 1].south);
      t[i][j]->req_tx[DIRECTION_SOUTH](req[i][j + 1].south);
      t[i][j]->ack_tx[DIRECTION_SOUTH](ack[i][j + 1].north);
      t[i][j]->buffer_full_status_tx[DIRECTION_SOUTH](buffer_full_status[i][j + 1].north);

      t[i][j]->evt_tx[DIRECTION_WEST](evt[i][j].west);
      t[i][j]->req_tx[DIRECTION_WEST](req[i][j].west);
      t[i][j]->ack_tx[DIRECTION_WEST](ack[i][j].east);
      t[i][j]->buffer_full_status_tx[DIRECTION_WEST](buffer_full_status[i][j].east);
    }
  }

  // Clear signals for boundary nodes

  for (int i = 0; i < GlobalParams::mesh_dim_x; ++i)
  {
    req[i][0].south = 0;
    ack[i][0].north = 0;
    req[i][GlobalParams::mesh_dim_y].north = 0;
    ack[i][GlobalParams::mesh_dim_y].south = 0;
  }

  for (int j = 0; j < GlobalParams::mesh_dim_y; ++j)
  {
    req[0][j].east = 0;
    ack[0][j].west = 0;
    req[GlobalParams::mesh_dim_x][j].west = 0;
    ack[GlobalParams::mesh_dim_x][j].east = 0;
  }
}

void NoC::buildTorus()
{
  // Note: The torus shaped network needs to be
  //       supported by the routing algorithm
  //       and thus the routing table used
  
  // Initialize signals
  int dimX = GlobalParams::mesh_dim_x;
  int dimY = GlobalParams::mesh_dim_y;

  // This method is similar to buildMesh()
  // except for connections at the boundary nodes

  evt = new sc_signal_NESW<AER_EVT>*[dimX];
  req = new sc_signal_NESW<bool>*[dimX];
  ack = new sc_signal_NESW<bool>*[dimX];
  buffer_full_status = new sc_signal_NESW<bool>*[dimX];

  for (int i = 0; i < dimX; ++i)
  {
    evt[i] = new sc_signal_NESW<AER_EVT>[dimY];
    req[i] = new sc_signal_NESW<bool>[dimY];
    ack[i] = new sc_signal_NESW<bool>[dimY];
    buffer_full_status[i] = new sc_signal_NESW<bool>[dimY];
  }

  t = new Tile**[GlobalParams::mesh_dim_x];
  for (int i = 0; i < GlobalParams::mesh_dim_x; ++i)
  {
    t[i] = new Tile*[GlobalParams::mesh_dim_y];
  }

  // Create the torus mesh as a matrix of tiles
  for (int j = 0; j < GlobalParams::mesh_dim_y; ++j)
  {
    for (int i = 0; i < GlobalParams::mesh_dim_x; ++i)
    {
      // Create a single tile with a proper name
      char tile_name[64];
      Coord tile_coord;
      tile_coord.x = i;
      tile_coord.y = j;
      int tile_id = coord2Id(tile_coord);
      sprintf(tile_name, "Tile[%02d][%02d]_(#%d)", i , j, tile_id);
      t[i][j] = new Tile(tile_name, tile_id);

      // Configure the router
      t[i][j]->r->configure(tile_id, GlobalParams::stats_warm_up_time,
			    GlobalParams::buffer_depth, grtable);
      t[i][j]->r->power.configureRouter(GlobalParams::evt_width, GlobalParams::buffer_depth,
					GlobalParams::evt_width);

      // Configure the processing element(s)
      for (int k = 0; k < NO_PES; ++k)
      {
	t[i][j]->pe[k]->tile_id = tile_id;
	t[i][j]->pe[k]->local_id = k;
	std::string spk_rec_fname;
	int min_n_id, max_n_id;
	if (gnat->getSpkRecFileParams(tile_id, k, spk_rec_fname, min_n_id, max_n_id))
	  t[i][j]->pe[k]->l_evt_q.init(spk_rec_fname, min_n_id, max_n_id);
	else
	  t[i][j]->pe[k]->l_evt_q.invalidate();
      }

      // Map clock and reset
      t[i][j]->clock(clock);
      t[i][j]->reset(reset);

      // Varibales to cover boundary effects
      int ip = (i + 1) % GlobalParams::mesh_dim_x;
      int jp = (j + 1) % GlobalParams::mesh_dim_y;

      // Use ip and jp instead of (i + 1) and (j + 1) while defining the connections
      //
      // Map RX signals
      t[i][j]->evt_rx[DIRECTION_NORTH](evt[i][j].south);
      t[i][j]->req_rx[DIRECTION_NORTH](req[i][j].south);
      t[i][j]->ack_rx[DIRECTION_NORTH](ack[i][j].north);
      t[i][j]->buffer_full_status_rx[DIRECTION_NORTH](buffer_full_status[i][j].north);

      t[i][j]->evt_rx[DIRECTION_EAST](evt[ip][j].west);
      t[i][j]->req_rx[DIRECTION_EAST](req[ip][j].west);
      t[i][j]->ack_rx[DIRECTION_EAST](ack[ip][j].east);
      t[i][j]->buffer_full_status_rx[DIRECTION_EAST](buffer_full_status[ip][j].east);

      t[i][j]->evt_rx[DIRECTION_SOUTH](evt[i][jp].north);
      t[i][j]->req_rx[DIRECTION_SOUTH](req[i][jp].north);
      t[i][j]->ack_rx[DIRECTION_SOUTH](ack[i][jp].south);
      t[i][j]->buffer_full_status_rx[DIRECTION_SOUTH](buffer_full_status[i][jp].south);

      t[i][j]->evt_rx[DIRECTION_WEST](evt[i][j].east);
      t[i][j]->req_rx[DIRECTION_WEST](req[i][j].east);
      t[i][j]->ack_rx[DIRECTION_WEST](ack[i][j].west);
      t[i][j]->buffer_full_status_rx[DIRECTION_WEST](buffer_full_status[i][j].west);

      // Map TX signals
      t[i][j]->evt_tx[DIRECTION_NORTH](evt[i][j].north);
      t[i][j]->req_tx[DIRECTION_NORTH](req[i][j].north);
      t[i][j]->ack_tx[DIRECTION_NORTH](ack[i][j].south);
      t[i][j]->buffer_full_status_tx[DIRECTION_NORTH](buffer_full_status[i][j].south);

      t[i][j]->evt_tx[DIRECTION_EAST](evt[ip][j].east);
      t[i][j]->req_tx[DIRECTION_EAST](req[ip][j].east);
      t[i][j]->ack_tx[DIRECTION_EAST](ack[ip][j].west);
      t[i][j]->buffer_full_status_tx[DIRECTION_EAST](buffer_full_status[ip][j].west);

      t[i][j]->evt_tx[DIRECTION_SOUTH](evt[i][jp].south);
      t[i][j]->req_tx[DIRECTION_SOUTH](req[i][jp].south);
      t[i][j]->ack_tx[DIRECTION_SOUTH](ack[i][jp].north);
      t[i][j]->buffer_full_status_tx[DIRECTION_SOUTH](buffer_full_status[i][jp].north);

      t[i][j]->evt_tx[DIRECTION_WEST](evt[i][j].west);
      t[i][j]->req_tx[DIRECTION_WEST](req[i][j].west);
      t[i][j]->ack_tx[DIRECTION_WEST](ack[i][j].east);
      t[i][j]->buffer_full_status_tx[DIRECTION_WEST](buffer_full_status[i][j].east);
    }
  }
}

Tile* NoC::searchNode(const int id) const
{
  for (int i = 0; i < GlobalParams::mesh_dim_x; ++i)
    for (int j = 0; j < GlobalParams::mesh_dim_y; ++j)
      if (t[i][j]->r->local_id == id)
	return t[i][j];

  return NULL;
}
