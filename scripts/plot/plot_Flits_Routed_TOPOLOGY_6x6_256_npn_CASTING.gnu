#####################################################################################
#                                                                                    
# NENoCSi-Framework: Network traffic within NENoCSi generated by NEST
# Copyright (C) 2022-2023 Forschungszentrum Juelich GmbH, ZEA-2                      
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
# along with this program.  If not, see <https://www.gnu.org/licenses/>.             
#____________________________________________________________________________________
#                                                                                    
# This script creates a heatmap of routed flits.                                     
#                                                                                    
# It needs to be called with                                                         
#  gnuplot -c <name of the script> <datadir> <topology> <casting> <val>              
# where <name of the script> is the file name of the script, <datadir> points to     
# the simulations data directory, <topology> is a string containing the topology,    
# <casting> is a string containing the abbreviation of the casting type, and <val>   
# is a numerical value indicating the acceleration factor (acc)                      
#                                                                                    
#####################################################################################
INFILE = sprintf('%s/Flits_Routed/Flits_Routed_%s_6x6_256_npn_%s_acc_%03d.csv', ARG1, ARG2, ARG3, ARG4 + 0)
OUTFILE = sprintf('%s/Images/Flits_Routed_%s_6x6_256_npn_%s_acc_%03d.pdf', ARG1, ARG2, ARG3, ARG4 + 0)
PLOT_TITLE = sprintf('Flits Routed - %s %s - Acc. Factor %d', ARG2, ARG3, ARG4 + 0)
set term pdfcairo
set output OUTFILE
set size square
set border lw 2
set xrange [-0.5:5.5]
set yrange [5.5:-0.5]
set xtics autofreq 0,1
set ytics autofreq 0,1
set title PLOT_TITLE noenhanced
set xlabel "x coord." font "Helvetica,12"
set ylabel "y coord." font "Helvetica,12"
plot INFILE w image notitle