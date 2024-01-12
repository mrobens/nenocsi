/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2018 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the implementation of the power model
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
 * 2022-09-11: There is no support for wireless networks in McAERsim, so that
 *             corresponding methods and method protions have not been implemented.
 *             At the same time, McAERsim uses a different router pipeline, so that 
 *             respective methods have been adapted. Also, multiple outputs can be 
 *             activated at once, so that the computation of dynamic crossbar power 
 *             needs to take the number of active outputs into account. The makro
 *             definition of W2J(...) has been shifted to Power.h.
 *
 *///******************************************************************************** 

#include "Power.h"

Power::Power()
{
  total_power = 0.0;
  buffer_router_push_pwr_d = 0.0;
  buffer_router_pop_pwr_d = 0.0;
  buffer_router_front_pwr_d = 0.0;
  buffer_router_pwr_s = 0.0;

  arbitration_pwr_d = 0.0;
  arbitration_pwr_s = 0.0;
  tcam_lookup_pwr_d = 0.0;
  tcam_lookup_pwr_s = 0.0;
  sram_lookup_pwr_d = 0.0;
  sram_lookup_pwr_s = 0.0;
  for (int k = 0; k < RADIX; ++k)
  {
    crossbar_pwr_d[k] = 0.0;
    crossbar_pwr_s[k] = 0.0;
  }
  link_r2r_pwr_d = 0.0;
  link_r2r_pwr_s = 0.0;
  ni_pwr_d = 0.0;
  ni_pwr_s = 0.0;

  initPowerBreakdown();
}

void Power::configureRouter(int link_width, int buffer_depth, int buffer_item_size)
{
  // (s)tatic, (d)ynamic power
  // Buffer
  std::pair<int, int> key = std::pair<int, int>(buffer_depth, buffer_item_size);
  assert(GlobalParams::power_configuration.bufferPowerConfig.leakage.find(key) != GlobalParams::power_configuration.bufferPowerConfig.leakage.end());
  assert(GlobalParams::power_configuration.bufferPowerConfig.push.find(key) != GlobalParams::power_configuration.bufferPowerConfig.push.end());
  assert(GlobalParams::power_configuration.bufferPowerConfig.front.find(key) != GlobalParams::power_configuration.bufferPowerConfig.front.end());
  assert(GlobalParams::power_configuration.bufferPowerConfig.pop.find(key) != GlobalParams::power_configuration.bufferPowerConfig.pop.end());

  // Dynamic values are expressed in Joule
  // Static/leakage values must be converted from Watt to Joule

  buffer_router_pwr_s = W2J(GlobalParams::power_configuration.bufferPowerConfig.leakage[key]);
  buffer_router_push_pwr_d = GlobalParams::power_configuration.bufferPowerConfig.push[key];
  buffer_router_front_pwr_d = GlobalParams::power_configuration.bufferPowerConfig.front[key];
  buffer_router_pop_pwr_d = GlobalParams::power_configuration.bufferPowerConfig.pop[key];

  // Arbitration, TCAM and SRAM lookup
  assert(GlobalParams::power_configuration.routerPowerConfig.arbitration_pm.find(buffer_item_size) != GlobalParams::power_configuration.routerPowerConfig.arbitration_pm.end());
  assert(GlobalParams::power_configuration.routerPowerConfig.tcam_pm.find(buffer_item_size) != GlobalParams::power_configuration.routerPowerConfig.tcam_pm.end());
  assert(GlobalParams::power_configuration.routerPowerConfig.sram_pm.find(buffer_item_size) != GlobalParams::power_configuration.routerPowerConfig.sram_pm.end());

  arbitration_pwr_s = W2J(GlobalParams::power_configuration.routerPowerConfig.arbitration_pm[buffer_item_size].first);
  arbitration_pwr_d = GlobalParams::power_configuration.routerPowerConfig.arbitration_pm[buffer_item_size].second;
  tcam_lookup_pwr_s = W2J(GlobalParams::power_configuration.routerPowerConfig.tcam_pm[buffer_item_size].first);
  tcam_lookup_pwr_d = GlobalParams::power_configuration.routerPowerConfig.tcam_pm[buffer_item_size].second;
  sram_lookup_pwr_s = W2J(GlobalParams::power_configuration.routerPowerConfig.sram_pm[buffer_item_size].first);
  sram_lookup_pwr_d = GlobalParams::power_configuration.routerPowerConfig.sram_pm[buffer_item_size].second;

  // CrossBar
  for (int k = 0; k < RADIX; ++k)
  {
    boost::tuples::tuple<int, int, int> xbar_k = boost::tuples::tuple<int, int, int>(RADIX, buffer_item_size, k + 1);
    assert(GlobalParams::power_configuration.routerPowerConfig.crossbar_pm.find(xbar_k) != GlobalParams::power_configuration.routerPowerConfig.crossbar_pm.end());
    crossbar_pwr_s[k] = W2J(GlobalParams::power_configuration.routerPowerConfig.crossbar_pm[xbar_k].first);
    crossbar_pwr_d[k] = GlobalParams::power_configuration.routerPowerConfig.crossbar_pm[xbar_k].second;
  }
    
  // Network interface
  assert(GlobalParams::power_configuration.routerPowerConfig.network_interface.find(link_width) != GlobalParams::power_configuration.routerPowerConfig.network_interface.end());
  ni_pwr_s = W2J(GlobalParams::power_configuration.routerPowerConfig.network_interface[link_width].first);
  ni_pwr_d = GlobalParams::power_configuration.routerPowerConfig.network_interface[link_width].second;

  // Link
  double length_r2r = GlobalParams::r2r_link_length;
  assert(GlobalParams::power_configuration.linkBitLinePowerConfig.find(length_r2r) != GlobalParams::power_configuration.linkBitLinePowerConfig.end());
  link_r2r_pwr_s = W2J(link_width * GlobalParams::power_configuration.linkBitLinePowerConfig[length_r2r].first);
  link_r2r_pwr_d = link_width * GlobalParams::power_configuration.linkBitLinePowerConfig[length_r2r].second;
}

// Router buffer
void Power::bufferRouterPush()
{
  power_dynamic.breakdown[BUFFER_PUSH_PWR_D].value += buffer_router_push_pwr_d;
}
void Power::bufferRouterFront()
{
  power_dynamic.breakdown[BUFFER_FRONT_PWR_D].value += buffer_router_front_pwr_d;
}
void Power::bufferRouterPop()
{
  power_dynamic.breakdown[BUFFER_POP_PWR_D].value += buffer_router_pop_pwr_d;
}
void Power::arbitration()
{
  power_dynamic.breakdown[ARBITRATION_PWR_D].value += arbitration_pwr_d;
}
void Power::tcam_lookup()
{
  power_dynamic.breakdown[TCAM_LOOKUP_PWR_D].value += tcam_lookup_pwr_d;
}
void Power::sram_lookup()
{
  power_dynamic.breakdown[SRAM_LOOKUP_PWR_D].value += sram_lookup_pwr_d;
}
void Power::crossbar(const int no_out)
{
  power_dynamic.breakdown[CROSSBAR_PWR_D].value += crossbar_pwr_d[no_out - 1];
}
void Power::r2rLink()
{
  power_dynamic.breakdown[LINK_R2R_PWR_D].value += link_r2r_pwr_d;
}
void Power::networkInterface()
{
  power_dynamic.breakdown[NI_PWR_D].value += ni_pwr_d;
}

void Power::leakageBufferRouter()
{
  power_static.breakdown[BUFFER_ROUTER_PWR_S].value += buffer_router_pwr_s;
}
void Power::leakageNetworkInterface()
{
  power_static.breakdown[NI_PWR_S].value += ni_pwr_s;
}
void Power::leakageRouter()
{
  // Note: Leakage contributions depending on instance number
  //       are accounted for in specific separate leakage functions
  power_static.breakdown[ARBITRATION_PWR_S].value += arbitration_pwr_s;
  power_static.breakdown[TCAM_PWR_S].value += tcam_lookup_pwr_s;
  power_static.breakdown[SRAM_PWR_S].value += sram_lookup_pwr_s;
  power_static.breakdown[CROSSBAR_PWR_S].value += crossbar_pwr_s[0];   // Should not depend on the number
}                                                                      // of active outputs

double Power::getDynamicPower()
{
  double power = 0.0;
  for (int i = 0; i < power_dynamic.size; ++i)
  {
    power += power_dynamic.breakdown[i].value;
  }
  return power;
}

double Power::getStaticPower()
{
  double power = 0.0;
  for (int i = 0; i < power_static.size; ++i)
  {
    power += power_static.breakdown[i].value;
  }
  return power;
}

void Power::initPowerBreakdownEntry(PowerBreakdownEntry* pbde, std::string label)
{
  pbde->label = label;
  pbde->value = 0.0;
}

void Power::initPowerBreakdown()
{
  power_dynamic.size = NO_BREAKDOWN_ENTRIES_D;
  power_static.size = NO_BREAKDOWN_ENTRIES_S;

  initPowerBreakdownEntry(&power_dynamic.breakdown[BUFFER_PUSH_PWR_D], "buffer_push_pwr_d");
  initPowerBreakdownEntry(&power_dynamic.breakdown[BUFFER_FRONT_PWR_D], "buffer_front_pwr_d");
  initPowerBreakdownEntry(&power_dynamic.breakdown[BUFFER_POP_PWR_D], "buffer_pop_pwr_d");
  initPowerBreakdownEntry(&power_dynamic.breakdown[ARBITRATION_PWR_D], "arbitration_pwr_d");
  initPowerBreakdownEntry(&power_dynamic.breakdown[TCAM_LOOKUP_PWR_D], "tcam_lookup_pwr_d");
  initPowerBreakdownEntry(&power_dynamic.breakdown[SRAM_LOOKUP_PWR_D], "sram_lookup_pwr_d");
  initPowerBreakdownEntry(&power_dynamic.breakdown[CROSSBAR_PWR_D], "crossbar_pwr_d");
  initPowerBreakdownEntry(&power_dynamic.breakdown[LINK_R2R_PWR_D], "link_r2r_pwr_d");
  initPowerBreakdownEntry(&power_dynamic.breakdown[NI_PWR_D], "ni_pwr_d");

  initPowerBreakdownEntry(&power_static.breakdown[BUFFER_ROUTER_PWR_S], "buffer_router_pwr_s");
  initPowerBreakdownEntry(&power_static.breakdown[ARBITRATION_PWR_S], "arbitration_pwr_s");
  initPowerBreakdownEntry(&power_static.breakdown[TCAM_PWR_S], "tcam_pwr_s");
  initPowerBreakdownEntry(&power_static.breakdown[SRAM_PWR_S], "sram_pwr_s");
  initPowerBreakdownEntry(&power_static.breakdown[CROSSBAR_PWR_S], "crossbar_pwr_s");
  initPowerBreakdownEntry(&power_static.breakdown[NI_PWR_S], "ni_pwr_s");
}
