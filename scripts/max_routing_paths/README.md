Maximum Routing Paths Lengths
=============================

Currently, maximum routing paths lengths are not saved and exported by the McAERsim framework. However, unless modified, a simulation of the cortical microcircuit model of NEST always uses the same random number generator seed ([Potjans\_2014/sim\_params.py](https://github.com/nest/nest-simulator/blob/master/pynest/examples/Potjans_2014/sim_params.py)). According to NEST's guide on parallel simulation, this always results in the same biological network being generated, as long as the same number of threads is used ([NEST parallel computing](https://nest-simulator.readthedocs.io/en/v3.1/guides/parallel_computing.html)). Thus, in case of deterministic XY routing, histograms of maximum routing paths lengths can be assembled by a Python script from the files used for data exchange between NEST and McAERsim. Examples of the respective connectivity and spike-recorder files for the cortical microcircuit model are available within the dataset at ([Zenodo](https://doi.org/10.5281/zenodo.10159252)).

To re-produce the histograms on maximum routing paths lengths within 6 &#x00D7; 6 mesh and torus networks for the cortical microcircuit model scaled to 10%, do the following:

1. Download the three script files from this subfolder to a convenient working directory.
2. Download the file `helpers.py` of the cortical microcircuit model of NEST into the same working directory. From the working directory, you may call the following command for this purpose:

            wget https://raw.githubusercontent.com/nest/nest-simulator/v3.1/pynest/examples/Potjans_2014/helpers.py

   Make sure to copy the whole line ending with `helpers.py`. For the functions that will be used, only the Python packages `os` and `numpy` are important. Thus, you may comment out `matplotlib`-related import commands.
3. Download the file `helpers_aux.py` from the `nest_inputs` subfolder of the NENoCSi branch within this repository. From the working directory, you may use the following command for this purpose:

           wget https://raw.githubusercontent.com/mrobens/nenocsi-mcaersim/nenocsi/nest_inputs/helpers_aux.py

   Make sure to copy the whole line ending with `helpers_aux.py`. For the function that will be used, only Python's `numpy` package is important. All other imports may be commented out.
4. Download the ZIP-file available at ([Zenodo](https://doi.org/10.5281/zenodo.10159252)).
5. Extract the subfolder `NoC-Sim-Input` to a convenient location.
6. Create the subfolders `data` and `img` in the working directory.
7. Execute `det_routing_McAERsim.sh` with the path to the parent directory of `NoC-Sim-Input` as argument.

By following these steps, two PDF images will be stored in the `img` subfolder created in step 6.
