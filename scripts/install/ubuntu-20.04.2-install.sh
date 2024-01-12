#!/bin/bash
##################################################################################################
#
# Noxim - the NoC Simulator
#
# (C) 2005-2018 by the University of Catania
# For the complete list of authors refer to file ../../doc/AUTHORS.txt
# For the license applied to these sources refer to file ../../doc/LICENSE.txt
#
#*************************************************************************************************
# Downloaded March 23, 2022 from                                                                  
# https://github.com/davidepatti/noxim/tree/c52ebce2217e57bcd4ff11a97b400323bd00acd5              
#*************************************************************************************************
#                                                                                                 
# McAERsim - NoC simulator with tree-based multicast support for AER packets                      
# Modifications Copyright (C) 2023 Forschungszentrum Juelich GmbH, ZEA-2                          
# Author: Markus Robens <https://www.fz-juelich.de/profile/robens_m>                              
# For the license applied to these modifications and McAERsim as a whole                          
# refer to file ../../doc/LICENSE_MCAERSIM.txt                                                    
#                                                                                                 
# 2023-03-27: To large extents, this Bash script contains new commands to download and install    
#             NEST as well as McAERsim and RTparser, if they are not already present. In addition,
#             PyNEST's cortical microcircuit model is copied to a convenient location, while two  
#             of its Python scripts are modified to enable the generation of a YAML file with     
#             connectivity information of NEST that can be converted to global routing tables for
#             McAERsim by RTparser. Thus, it automates the streamlined procedure proposed in      
#             ../../doc/INSTALL.txt. For more detailed information, please also consult the       
#             documentation of the single simulators at                                           
#             https://github.com/davidepatti/noxim/blob/master/doc/INSTALL.txt                 and
#             https://nest-simulator.readthedocs.io/en/v3.1/installation/linux_install.html       
#                                                                                                 
##################################################################################################
#
# Customization part
# ==================
#
# By default, this script will install the NEST-McAERsim framework into the "nest-3.1" and
# "mcaersim-1.0" subfolders of your home directory. The build process and installation are only
# performed, if respective folders don't exist. This way, NEST-McAERsim and NEST-NENoCSi
# frameworks can be installed side-by-side.
#
# Set installation directories
INST_DIR=${HOME}
NEST_INST=${INST_DIR}/nest-3.1
#
# Automatically processed part - shouldn't be modified
# ====================================================
# Additional directories
NEST_BUILD=${INST_DIR}/nest-3.1-build
NEST_SRC=${HOME}/Downloads/nest-simulator-3.1
NEST_MCAERSIM_ROOT=${INST_DIR}/mcaersim-1.0
#
# Get and build NEST if necessary
if ! [ -d ${NEST_INST} ]; then
    if ([ -d ${NEST_SRC} ] || [ -d ${NEST_BUILD} ]); then
	echo ""
	echo "Fragments of a former NEST installation still exist, can't proceed!"
	echo "You may want to delete them or install to a different directory."
	echo ""
	exit 1
    fi
echo ""
echo "Installing NEST 3.1 to ${NEST_INST}"
echo ""
cd ${NEST_SRC%/nest-simulator-3.1}
wget -O nest-simulator-3.1.tar.gz https://github.com/nest/nest-simulator/archive/refs/tags/v3.1.tar.gz
tar -xzf nest-simulator-3.1.tar.gz
# Configure and build NEST
PYVER=$(python3 -V | sed "s/^Python \([0-9]\.[0-9]\+\)\.[0-9]\+$/\1/")
mkdir ${NEST_INST}
mkdir ${NEST_BUILD}
cd ${NEST_BUILD}
cmake -DCMAKE_INSTALL_PREFIX:PATH=$NEST_INST -DCMAKE_BUILD_TYPE:STRING=Debug $NEST_SRC
make
export PYTHONPATH="${NEST_INST}/lib/python${PYVER}/site-packages${PYTHONPATH:+:$PYTHONPATH}"
make install
export PATH="${NEST_INST}/bin:${PATH}"
PYTHONUSERBASE=${NEST_INST} pip3 install --user junitparser
make installcheck
# Set additional NEST environment variable
cd ${NEST_INST}/bin
sed -i "\!export PATH=! a\\
\\
\# Set the location of user specific Python site-packages.\\
export PYTHONUSERBASE=\"${NEST_INST}\${PYTHONUSERBASE:+:\$PYTHONUSERBASE}\"
" nest_vars.sh
rm "${NEST_SRC}.tar.gz"
rm -r ${NEST_SRC}
rm -r ${NEST_BUILD}
fi
#
cd ${NEST_INST}/bin
source nest_vars.sh
# McAERsim
if ! [ -d ${NEST_MCAERSIM_ROOT} ]; then
echo ""
echo "Installing McAERsim to ${NEST_MCAERSIM_ROOT}"
echo ""
cd ${HOME}/Downloads
wget -O mcaersim-1.0.tar.gz https://github.com/mrobens/nenocsi-mcaersim/archive/refs/tags/mcaersim-1.0.tar.gz
# Extract and build McAERsim
mkdir ${NEST_MCAERSIM_ROOT}
cd ${NEST_MCAERSIM_ROOT}
tar --strip-components=1 -xzf ${HOME}/Downloads/mcaersim-1.0.tar.gz
cd bin
make depend
make
make rtparser
cd ${NEST_MCAERSIM_ROOT}/nest_inputs
cp ${NEST_INST}/share/doc/nest/examples/pynest/Potjans_2014/*.py .
sed -i "\!import warnings! a\\
import helpers_aux
\!helpers\.boxplot(self\.data_path, self\.net_dict\['populations'\])! a\\
            if(self.sim_dict['gen_yaml']):\\
                print('Exporting to ' + self.sim_dict['export_fname'])\\
                helpers_aux.export_connections(self.data_path, self.sim_dict['export_fname'], self.pops)
" network.py
sed -i "\!sim_dict = {! a\\
    \#YAML file name used for data export\\
    'export_fname': 'NEST_MC_Connections.yaml',\\
    \#Select if YAML output is created or not\\
    'gen_yaml': True,
" sim_params.py
python3 run_microcircuit.py
cd ${NEST_MCAERSIM_ROOT}/bin
./rtparser ../nest_inputs/data/population_nodeids.dat ../nest_inputs/data/NEST_MC_Connections.yaml
sed -i "s/^\(topology:\) TOPOLOGY_MESH$/\1 TOPOLOGY_TORUS/
s/^\(output_file_name:\) NEST_MC_Global_RT_Mesh.yaml/\1 NEST_MC_Global_RT_Torus.yaml/
" parser_config.yaml
./rtparser ../nest_inputs/data/population_nodeids.dat ../nest_inputs/data/NEST_MC_Connections.yaml
rm ${HOME}/Downloads/mcaersim-1.0.tar.gz
cd ${NEST_MCAERSIM_ROOT}/scripts
chmod u+x aux_scripts/*.sh
chmod u+x *.sh
fi
cd ${NEST_MCAERSIM_ROOT}/scripts
