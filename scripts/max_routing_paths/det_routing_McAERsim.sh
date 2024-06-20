#!/bin/bash
#************************************************************************
#
# McAERsim-Framework: Network traffic within McAERsim generated by NEST
# Copyright (C) 2024 Forschungszentrum Juelich GmbH, ZEA-2
# Author: Markus Robens <https://www.fz-juelich.de/profile/robens_m>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>
#
#************************************************************************
#
# This Bash script automates the creation of histogram data files on
# maximum routing paths lengths for multicast routing and different
# topologies. It generates respective plots for each topology as well.
#
# Note: This script needs to receive the path to the NoC-Sim-Input
#       subfolder extracted from the ZIP-file available at
#       https://doi.org/10.5281/zenodo.10159252 or an equally
#       organized dataset created by the McAERsim framework as a
#       parameter.
#
#************************************************************************
APATH="${1}/NoC-Sim-Input/1PE_Scale_0_10/MC/"
for TOPO in "mesh" "torus"
do
   python3 RoutingPathsMcAERsim.py ${APATH} ${TOPO} 256 6 6
   mv LinkLengthHisto${TOPO^?}MC.csv ./data
   gnuplot -c plotLL-McAERsim.gnu ${TOPO^?} Log
done
