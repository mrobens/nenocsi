/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2018 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the declaration of the power model
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
 * 2022-09-11: In McAERsim, there is no support for different routing and selection
 *             functions as well as wireless connections. Methods and attributes
 *             have been simplified accordingly. Since a different router pipeline
 *             model is used, methods and attributes have been adapted. Energy
 *             consumed by the crossbar has been made variable, because varying
 *             numbers of active outputs are possible. The makro definition of
 *             W2J(...) has been shifted from Power.cpp to Power.h.
 *
 *///******************************************************************************** 

#ifndef __MCAERSIMPOWER_H__
#define __MCAERSIMPOWER_H__

#include <cassert>
#include <fstream>
#include "DataStructs.h"
#include "GlobalParams.h"

#define W2J(watt) ((watt)*GlobalParams::clock_period_ps*1.0e-12);

struct Power
{
  Power();
  void configureRouter(int link_width, int buffer_depth, int buffer_item_size);
  
  void bufferRouterPush();
  void bufferRouterPop();
  void bufferRouterFront();
  void arbitration();
  void tcam_lookup();
  void sram_lookup();
  void crossbar(const int no_out);
  void r2rLink();
  void networkInterface();

  void leakageBufferRouter();
  void leakageNetworkInterface();
  void leakageRouter();

  double getDynamicPower();
  double getStaticPower();
  double getTotalPower() {return (getDynamicPower() + getStaticPower());}

  PowerBreakdown* getDynamicPowerBreakdown() {return &power_dynamic;}
  PowerBreakdown* getStaticPowerBreakdown() {return &power_static;}

 private:

  double total_power;
  double buffer_router_push_pwr_d;
  double buffer_router_pop_pwr_d;
  double buffer_router_front_pwr_d;
  double buffer_router_pwr_s;

  double arbitration_pwr_d;
  double arbitration_pwr_s;
  double tcam_lookup_pwr_d;
  double tcam_lookup_pwr_s;
  double sram_lookup_pwr_d;
  double sram_lookup_pwr_s;
  double crossbar_pwr_d[RADIX];
  double crossbar_pwr_s[RADIX];
  double link_r2r_pwr_d;
  double link_r2r_pwr_s;
  double ni_pwr_d;
  double ni_pwr_s;

  PowerBreakdown power_dynamic;
  PowerBreakdown power_static;
  void initPowerBreakdownEntry(PowerBreakdownEntry* pbde, std::string label);
  void initPowerBreakdown();
};

#endif /* __MCAERSIMPOWER_H__ */
