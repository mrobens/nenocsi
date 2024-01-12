/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2018 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the implementaton of the global statistics
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
 * 2022-10-06: Neither delta topologies nor wireless transmissions are supported
 *             by McAERsim. Related methods or method portions therefore have
 *             not been implemented. On the other hand, support for multiple
 *             processing elements per tile has been included. Methods 
 *             getReceivedPackets() and getReceivedFlits() have been replaced
 *             by method getReceivedEvts(). The ratio beetween the number of 
 *             ideal and the number of actually received events is hard to 
 *             determine in the multicast case and has thus been omitted. However, 
 *             a method returning the number of local deliveries is additionally 
 *             provided, which can be used to compare the number of received events 
 *             against the number of received flits in the source-address driven 
 *             local multicast case of NENoCSi to make sure, that all events 
 *             got delivered. Furthermore, methods checkCamFailure(), 
 *             setOverallDelays(...) and getOverallQueuedEvts() have been added.
 *             Finally, a means to export the delay vector as well as the matrix
 *             of routed events has been realized.  
 *
 *///******************************************************************************** 

#include "GlobalStats.h"

void GlobalStats::updatePowerBreakdown(std::map<std::string, double>& dst, PowerBreakdown* src)
{
  for (int i = 0; i != src->size; ++i)
  {
    dst[src->breakdown[i].label] += src->breakdown[i].value;
  }
}

GlobalStats::GlobalStats(const NoC* _noc)
{
  noc = _noc;
}

bool GlobalStats::checkCamFailure()
{
  bool CAM_lookup_failed = false;

  for (int y = 0; y < GlobalParams::mesh_dim_y; ++y)
    for (int x = 0; x < GlobalParams::mesh_dim_x; ++x)
      CAM_lookup_failed |= noc->t[x][y]->r->checkCamFailure();

  return CAM_lookup_failed;
}

double GlobalStats::getAverageDelay()
{
  unsigned int total_evts = 0;
  double avg_delay = 0.0;

  for (int y = 0; y < GlobalParams::mesh_dim_y; ++y)
    for (int x = 0; x < GlobalParams::mesh_dim_x; ++x)
      for (int k = 0; k < NO_PES; ++k)
      {
        unsigned int received_evts = noc->t[x][y]->r->stats.getReceivedEvts(k);
        if (received_evts)
        {
	  avg_delay += received_evts * noc->t[x][y]->r->stats.getAverageDelay(k);
	  total_evts += received_evts;
        }
      }

  avg_delay /= (double) total_evts;
  return avg_delay;
}

double GlobalStats::getAverageDelay(const int src_id, const int dst_id)
{
  int dst_tile_id = dst_id / (int) NO_PES;
  int dst_pe_id = dst_id % (int) NO_PES;
  Tile *tile = noc->searchNode(dst_tile_id);
  assert(tile != NULL);
  return tile->r->stats.getAverageDelay(src_id, dst_pe_id);
}

double GlobalStats::getMaxDelay()
{
  double maxd = -1.0;
  for (int y = 0; y < GlobalParams::mesh_dim_y; ++y)
    for (int x = 0; x < GlobalParams::mesh_dim_x; ++x)
      for (int k = 0; k < NO_PES; ++k)
      {
        Coord coord;
        coord.x = x;
        coord.y = y;
        int tile_id = coord2Id(coord);
	int g_src_id = (tile_id * NO_PES) + k;
        double d = getMaxDelay(g_src_id);
        if (d > maxd)
	  maxd = d;
      }

  return maxd;
}

double GlobalStats::getMaxDelay(const int dst_id)
{
  int tile_id = dst_id / (int) NO_PES;
  int l_pe_id = dst_id % (int) NO_PES;

  Coord coord = id2Coord(tile_id);
  unsigned int received_evts = noc->t[coord.x][coord.y]->r->stats.getReceivedEvts(l_pe_id);
  if (received_evts)
    return noc->t[coord.x][coord.y]->r->stats.getMaxDelay(l_pe_id);
  else
    return -1.0;
}

double GlobalStats::getMaxDelay(const int src_id, const int dst_id)
{
  int tile_id = dst_id / (int) NO_PES;
  int l_pe_id = dst_id % (int) NO_PES;

  Tile* tile = noc->searchNode(tile_id);
  assert(tile != NULL);
  return tile->r->stats.getMaxDelay(src_id, l_pe_id);
}

std::vector<std::vector<double> > GlobalStats::getMaxDelayMtx()
{
  std::vector<std::vector<double> > mtx;
  int p_dimX = GlobalParams::proc_arr_dim_x;
  int p_dimY = GlobalParams::proc_arr_dim_y;
  int mat_dim_x = GlobalParams::mesh_dim_x * p_dimX;
  int mat_dim_y = GlobalParams::mesh_dim_y * p_dimY;

  mtx.resize(mat_dim_y);
  for (int y = 0; y < mat_dim_y; ++y)
    mtx[y].resize(mat_dim_x);

  for (int m_y = 0; m_y < GlobalParams::mesh_dim_y; ++m_y)
    for (int m_x = 0; m_x < GlobalParams::mesh_dim_x; ++m_x)
    {
      Coord coord;
      coord.x = m_x;
      coord.y = m_y;
      int tile_id = coord2Id(coord);
      for (int p_y = 0; p_y < p_dimY; ++p_y)
	for (int p_x = 0; p_x < p_dimX; ++p_x)
	{
	  int l_pe_id = (p_y * p_dimX) + p_x;
	  int g_pe_id = (tile_id * NO_PES) + l_pe_id;
	  mtx[m_y * p_dimY + p_y][m_x * p_dimX + p_x] = getMaxDelay(g_pe_id);
	}
    }

  return mtx;
}

void GlobalStats::setOverallDelays(std::vector<double>& overall_delays) const
{
  std::vector<std::vector<double> > tmp_delays;
  for (int y = 0; y < GlobalParams::mesh_dim_y; ++y)
    for (int x = 0; x < GlobalParams::mesh_dim_x; ++x)
    {
      noc->t[x][y]->r->stats.setDelays(tmp_delays);
      for (int k = 0; k < NO_PES; ++k)
	if (tmp_delays[k].size() > 0)
	  overall_delays.insert(overall_delays.end(), tmp_delays[k].begin(), tmp_delays[k].end());
    }
}

long GlobalStats::getOverallQueuedEvts() const
{
  long queuedEvts = 0;
  for (int y = 0; y < GlobalParams::mesh_dim_y; ++y)
    for (int x = 0; x < GlobalParams::mesh_dim_x; ++x)
      for (int k = 0; k < NO_PES; ++k)
	queuedEvts += noc->t[x][y]->pe[k]->getQueuedAEREvts();

  return queuedEvts;
}

double GlobalStats::getAverageThroughput(const int src_id, const int dst_id)
{
  int tile_id = dst_id / (int) NO_PES;
  int l_pe_id = dst_id % (int) NO_PES;

  Tile* tile = noc->searchNode(tile_id);
  assert(tile != NULL);
  return tile->r->stats.getAverageThroughput(src_id, l_pe_id);
}

double GlobalStats::getAggregatedThroughput()
{
  int total_cycles = GlobalParams::simulation_time - GlobalParams::stats_warm_up_time;
  return (double)getReceivedEvts() / (double)(total_cycles);
}

double GlobalStats::getThroughput()
{
  int number_of_ip = GlobalParams::mesh_dim_x * GlobalParams::mesh_dim_y * NO_PES;
  return (double)getAggregatedThroughput() / (double)(number_of_ip);
}

// Only accounting for IP that received at least one flit
double GlobalStats::getActiveThroughput()
{
  int total_cycles = GlobalParams::simulation_time - GlobalParams::stats_warm_up_time;
  unsigned int n_active = 0;
  unsigned int received_evts = 0;
  unsigned int total_received_evts = 0;
  for (int y = 0; y < GlobalParams::mesh_dim_y; ++y)
    for (int x = 0; x < GlobalParams::mesh_dim_x; ++x)
      for (int k = 0; k < NO_PES; ++k)
      {
	received_evts = noc->t[x][y]->r->stats.getReceivedEvts(k);
	if (received_evts != 0)
	  ++n_active;

	total_received_evts += received_evts;
      }

  return (double) total_received_evts / (double) (total_cycles * n_active);
}

unsigned int GlobalStats::getReceivedEvts()
{
  unsigned int n = 0;
  for (int y = 0; y < GlobalParams::mesh_dim_y; ++y)
    for (int x = 0; x < GlobalParams::mesh_dim_x; ++x)
      for (int k = 0; k < NO_PES; ++k)
	n += noc->t[x][y]->r->stats.getReceivedEvts(k);

  return n;
}

unsigned int GlobalStats::getLocalDeliveries()
{
  unsigned int n = 0;
  for (int y = 0; y < GlobalParams::mesh_dim_y; ++y)
    for (int x = 0; x < GlobalParams::mesh_dim_x; ++x)
      n += noc->t[x][y]->r->stats.getLocalDeliveries();

  return n;
}

std::vector<std::vector<unsigned long> > GlobalStats::getRoutedEvtsMtx()
{
  std::vector<std::vector<unsigned long> > mtx;
  mtx.resize(GlobalParams::mesh_dim_y);
  for (int y = 0; y < GlobalParams::mesh_dim_y; ++y)
    mtx[y].resize(GlobalParams::mesh_dim_x);

  for (int y = 0; y < GlobalParams::mesh_dim_y; ++y)
    for (int x = 0; x < GlobalParams::mesh_dim_x; ++x)
      mtx[y][x] = noc->t[x][y]->r->getRoutedEvts();

  return mtx;
}

double GlobalStats::getDynamicPower()
{
  double power = 0.0;
  for (int y = 0; y < GlobalParams::mesh_dim_y; ++y)
    for (int x = 0; x < GlobalParams::mesh_dim_x; ++x)
      power += noc->t[x][y]->r->power.getDynamicPower();    // The router accounts for all power contributions
                                                            // of all elements, including network interfaces
  return power;
}

double GlobalStats::getStaticPower()
{
  double power = 0.0;
  for (int y = 0; y < GlobalParams::mesh_dim_y; ++y)
    for (int x = 0; x < GlobalParams::mesh_dim_x; ++x)
      power += noc->t[x][y]->r->power.getStaticPower();

  return power;
}
	  
void GlobalStats::showStats(std::ostream& out, bool detailed)
{
  if (detailed)
  {
    out << std::endl << "detailed = [" << std::endl;
    for (int y = 0; y < GlobalParams::mesh_dim_y; ++y)
      for (int x = 0; x < GlobalParams::mesh_dim_x; ++x)
	noc->t[x][y]->r->stats.showStats(out, true);

    out << "];" << std::endl;

    // Show MaxDelay matrix
    std::vector<std::vector<double> > md_mtx = getMaxDelayMtx();
    out << std::endl << "max_delay = [" << std::endl;
    for (unsigned int y = 0; y < md_mtx.size(); ++y)
    {
      out << "    ";
      for (unsigned int x = 0; x < md_mtx[y].size(); ++x)
	out << std::setw(6) << md_mtx[y][x];

      out << std::endl;
    }
    out << "];" << std::endl;

    // Show RoutedEvts matrix
    std::vector<std::vector<unsigned long> > re_mtx = getRoutedEvtsMtx();
    out << std::endl << "routed_evts = [" << std::endl;
    for (unsigned int y = 0; y < re_mtx.size(); ++y)
    {
      out << "    ";
      for (unsigned int x = 0; x < re_mtx[y].size(); ++x)
	out << std::setw(10) << re_mtx[y][x];

      out << std::endl;
    }
    out << "];" << std::endl;

    // Show power breakdown
    showPowerBreakdown(out);
  }

  out << "% Total received events: " << getReceivedEvts() << std::endl;
  out << "% Locally delivered events: " << getLocalDeliveries() << std::endl;
  out << "% Global average delay (cycles): " << getAverageDelay() << std::endl;
  out << "% Maximum delay (cycles): " << getMaxDelay() << std::endl;
  out << "% Network throughput (evts/cycle): " << getAggregatedThroughput() << std::endl;
  out << "% Average PE throughput (evts/cycle/PE): " << getThroughput() << std::endl;
  out << "% Total energy (J): " << getTotalPower() << std::endl;
  out << "% \tDynamic energy (J): " << getDynamicPower() << std::endl;
  out << "% \tStatic energy (J): " << getStaticPower() << std::endl;
  out << "% CAM look-up failure: " << (checkCamFailure() ? "true" : "false") << std::endl;
  out << std::endl;

  if (GlobalParams::create_output_files)
  {
    setOverallDelays(overall_delays);
    assert(exportStatData());
  }
  if (GlobalParams::show_buffer_stats)
    showBufferStats(out);
}

void GlobalStats::showBufferStats(std::ostream& out)
{
  out << std::endl;
  out << "Buffer Statistics (invalid buffers are blanked out)" << std::endl;
  out << std::left << std::setw(16) << "Router ID" << std::setw(16) << "Buffer North" << std::setw(16) << "Buffer East";
  out << std::setw(16) << "Buffer South" << std::setw(16) << "Buffer West";
  for (int k = 0; k < NO_PES; ++k)
  {
    std::string pe_name = std::string("Buffer PE").append(std::to_string(k));
    out << std::setw(16) << pe_name;
  }
  out << std::endl;
  out << std::left << std::setw(16) << "" << std::setw(8) << "Mean" << std::setw(8) << "Max" << std::setw(8) << "Mean" << std::setw(8) << "Max";
  out << std::setw(8) << "Mean" << std::setw(8) << "Max" << std::setw(8) << "Mean" << std::setw(8) << "Max";
  for (int k = 0; k < NO_PES; ++k)
    out << std::setw(8) << "Mean" << std::setw(8) << "Max";

  out << std::endl;
  for (int y = 0; y < GlobalParams::mesh_dim_y; ++y)
    for (int x = 0; x < GlobalParams::mesh_dim_x; ++x)
    {
      out << std::setw(16) << noc->t[x][y]->r->local_id;
      noc->t[x][y]->r->ShowBufferStats(out);
    }
}

void GlobalStats::showPowerBreakdown(std::ostream& out)
{
  std::map<std::string, double> power_dynamic;
  std::map<std::string, double> power_static;

  for (int y = 0; y < GlobalParams::mesh_dim_y; ++y)
    for (int x = 0; x < GlobalParams::mesh_dim_x; ++x)
    {
      updatePowerBreakdown(power_dynamic, noc->t[x][y]->r->power.getDynamicPowerBreakdown());
      updatePowerBreakdown(power_static, noc->t[x][y]->r->power.getStaticPowerBreakdown());
    }

  printMap("power_dynamic", power_dynamic, out);
  printMap("power_static", power_static, out);
}
  
bool GlobalStats::exportStatData(void)
{
  std::string delayOut = (std::string("Delays").append(GlobalParams::output_file_suffix)).append(".csv");
  std::string routedEvts = (std::string("Evts_Routed").append(GlobalParams::output_file_suffix)).append(".csv");
  boost::filesystem::path data("data");
  try
  {
    bool created = boost::filesystem::create_directory(data);
    if (created) std::cout << "Created folder 'data' used for output files" << std::endl;
    else std::cout << "Folder 'data' already existed. Using it for output files" << std::endl;
  }
  catch (const boost::filesystem::filesystem_error& fse)
  {
    std::cout << fse.what() << std::endl;
    return false;
  }
  boost::filesystem::path data1 = data;
  std::ofstream of_dout((data1 /= delayOut).c_str());
  if (!of_dout)
    return false;
  if(overall_delays.size() > 0)
  {
    of_dout << "#Delays collected from all nodes" << std::endl;
    for (unsigned int i = 0; i < overall_delays.size(); ++i)
      of_dout << overall_delays[i] << std::endl;
  }
  else
    std::cout << "Vector overall_delays empty. Did you call setOverallDelays(...)?" << std::endl;
  of_dout.close();
  boost::filesystem::path data2 = data;
  std::ofstream of_rtdout((data2 /= routedEvts).c_str());
  if (!of_rtdout)
    return false;
  of_rtdout << "#Matrix containing the number of routed flits per router" << std::endl;
  std::vector<std::vector<unsigned long> > evts_routed = getRoutedEvtsMtx();
  for (unsigned int y = 0; y < evts_routed.size(); ++y)
  {
    for (unsigned int x = 0; x < evts_routed[y].size(); ++x)
      of_rtdout << x << ", " << y << ", " << evts_routed[y][x] << std::endl;

    of_rtdout << std::endl;
  }
  of_rtdout.close();
  return true;
}
