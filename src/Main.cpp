/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2018 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the implementation of the top-level of Noxim
 */
/*\\\********************************************************************************
 * Downloaded March 23, 2022 from
 * https://github.com/davidepatti/noxim/tree/c52ebce2217e57bcd4ff11a97b400323bd00acd5
 * Updated December 13, 2023 according to
 * https://github.com/davidepatti/noxim/tree/afb4aa0c7e81aa37f023b46a9bbc2856a4ae6b2f
 ************************************************************************************
 *
 * McAERsim - NoC simulator with tree-based multicast support for AER packets
 * Modifications Copyright (C) 2022-2023 Forschungszentrum Juelich GmbH, ZEA-2
 * Author: Markus Robens <https://www.fz-juelich.de/profile/robens_m>
 * For the license applied to these modifications and McAERsim as a whole
 * refer to file ../doc/LICENSE_MCAERSIM.txt
 *
 * 2022-09-09: In McAERsim, there is no trace file support and no possibility to
 *             stop a simulation based on drained volume.
 *             The command line output has been appended to reveal that it is
 *             a modified program version. In addition, some small edits have
 *             been performed.
 *
 *///******************************************************************************** 

#include "ConfigurationManager.h"
#include "NoC.h"
#include "DataStructs.h"
#include "GlobalParams.h"
#include "GlobalStats.h"

#include <csignal>

NoC* n;

void signalHandler(int signum)
{
  int now = sc_time_stamp().to_double() / GlobalParams::clock_period_ps;
  std::cout << "\b\b  " << std::endl;
  std::cout << std::endl;
  std::cout << "Current Statistics:" << std::endl;
  std::cout << "(" << now << " sim cycles executed)" << std::endl;
  GlobalStats gs(n);
  gs.showStats(std::cout, GlobalParams::detailed);
}

int sc_main(int argc, char* argv[])
{
  signal(SIGQUIT, signalHandler);

  std::cout << "\t---------------------------------------------------------" << endl;
  std::cout << "\t\tNoxim - the NoC Simulator" << endl;
  std::cout << "\t\t(C) University of Catania" << endl;
  std::cout << "\t---------------------------------------------------------" << endl;

  std::cout << "    Catania V., Mineo A., Monteleone S., Palesi M., and Patti D." << endl;
  std::cout << "    (2016) Cycle-Accurate Network on Chip Simulation with Noxim." << endl;
  std::cout << "    ACM Trans. Model. Comput. Simul. 27, 1, Article 4 (August 2016)," << endl;
  std::cout << "    25 pages. DOI: https://doi.org/10.1145/2953878" << endl;
  std::cout << endl;
  std::cout << " -----------------------------------------------------------------------" << std::endl;
  std::cout << " McAERsim - NoC simulator with multicast support for AER packets" << std::endl;
  std::cout << " Modifications Copyright 2022-2023 Forschungszentrum Juelich GmbH, ZEA-2" << std::endl;
  std::cout << std::endl;
  std::cout << " This program is provided AS IS and comes with ABSOLUTELY NO WARRANTY." << std::endl;
  std::cout << " It is free software, and you are welcome to redistribute it under" << std::endl;
  std::cout << " certain conditions. See the LICENSE_MCAERSIM.txt file for details." << std::endl;
  std::cout << " -----------------------------------------------------------------------" << std::endl;

  configure(argc, argv);

  // Signals
  sc_clock clock("clock", GlobalParams::clock_period_ps, SC_PS);
  sc_signal<bool> reset;

  // NoC instance
  n = new NoC("NoC");
  n->clock(clock);
  n->reset(reset);

  // Reset the chip and run the simulation
  reset.write(1);
  std::cout << "Reset for " << (int)(GlobalParams::reset_time) << " cycles ... ";
  srand(GlobalParams::rnd_generator_seed);
  
  // fix clock periods different from 1ns
  //sc_start(GlobalParams::reset_time, SC_NS);
  sc_start(GlobalParams::reset_time * GlobalParams::clock_period_ps, SC_PS);

  reset.write(0);
  std::cout << " done! " << std::endl;
  std::cout << "Now running for " << GlobalParams::simulation_time << " cycles..." << std::endl;
  // fix clock periods different from 1ns
  //sc_start(GlobalParams::simulation_time, SC_NS);
  sc_start(GlobalParams::simulation_time * GlobalParams::clock_period_ps, SC_PS);

  std::cout << "McAERsim simulation completed.";
  std::cout << " (" << sc_time_stamp().to_double() / GlobalParams::clock_period_ps;
  std::cout << " cycles executed)" << std::endl;
  std::cout << "Simulation covers " << sc_time_stamp().to_seconds() << "s" << std::endl;
  std::cout << endl;
  // Show statistics
  GlobalStats gs(n);
  gs.showStats(std::cout, GlobalParams::detailed);
  return 0;
}
